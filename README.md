# CrudApp 구현 설명

JSON 파일을 데이터 저장소로 사용하는 C++ 콘솔 CRUD 애플리케이션입니다.

## 프로젝트 구성

```
PoC Sample.slnx          솔루션 파일 (CrudApp 프로젝트 등록됨)
CrudApp/
  CrudApp.vcxproj        Visual Studio 프로젝트 파일 (C++17, /utf-8)
  CrudApp.vcxproj.user   디버그 실행 시 작업 디렉터리를 프로젝트 폴더로 지정
  Json.h / Json.cpp      범용 JSON 파서 & 직렬화 라이브러리 (외부 의존성 없음)
  Record.h               데이터 모델(Record) 정의
  Repository.h / .cpp    JSON 파일 기반 CRUD 저장소 클래스
  main.cpp               콘솔 메뉴 UI 및 입력 처리
  data/records.json      실제 데이터가 저장되는 파일 (초기값 "[]")
```

## 왜 직접 JSON 파서를 구현했는가

nlohmann/json 같은 서드파티 라이브러리는 vcpkg/NuGet 등으로 받아야 하는데,
이 환경에서는 인터넷을 통한 패키지 설치가 불확실하여 외부 의존성 없이
`Json.h`/`Json.cpp`에 최소 기능의 JSON 파서 및 직렬화 로직을 직접 작성했습니다.
`null/bool/number/string/array/object` 타입을 모두 지원하고, object는 `vector<pair<string, JsonValue>>`로
저장하여 파일에 쓸 때 필드 순서(id → name → description → createdAt)가 유지되도록 했습니다.

## 데이터 모델 (Record.h)

```cpp
struct Record {
    int id;
    std::string name;
    std::string description;
    std::string createdAt; // "YYYY-MM-DD HH:MM:SS"
};
```

이번 구현은 범용 예시 모델(Id/Name/Description/CreatedAt)로 만들었습니다.
실제 사용할 필드가 정해지면 `Record` 구조체와 `toJson()`/`fromJson()`만 수정하면 되고,
`Repository`와 `main.cpp`의 나머지 로직은 그대로 재사용할 수 있도록 구조를 분리했습니다.

## Repository (Repository.h / Repository.cpp) — 저장소 계층

`records.json` 파일과 메모리상의 `std::vector<Record>` 사이를 중개하는 클래스입니다.

| 메서드 | 역할 |
|---|---|
| `load()` | 프로그램 시작 시 JSON 파일을 읽어 메모리에 적재. 파일이 없으면 빈 목록으로 시작하고, JSON이 손상되어 있으면 오류 메시지를 출력한 뒤 빈 목록으로 시작 (프로그램이 죽지 않도록 방어) |
| `save()` | 현재 메모리 상태를 JSON으로 직렬화하여 파일에 덮어쓰기. 모든 Create/Update/Delete 이후 자동 호출되어 항상 파일과 메모리 상태가 일치 |
| `listAll()` | 전체 레코드 목록 반환 (Read - 전체 조회) |
| `findById(id)` | ID로 단건 조회 (Read - ID 검색) |
| `searchByNameKeyword(keyword)` | 이름에 키워드가 포함된 레코드를 대소문자 구분 없이 검색 (Read - 키워드 검색) |
| `create(name, description)` | `id`는 기존 최대값+1로 자동 부여, `createdAt`은 현재 시각으로 자동 기록 후 저장 (Create) |
| `updateFields(id, newName, newDescription)` | `std::optional`로 받아 값이 있는 필드만 수정. 두 값 다 `nullopt`면 아무것도 바꾸지 않음 (Update) |
| `removeById(id)` | ID로 레코드를 찾아 삭제 후 저장 (Delete) |

## Update(수정) 방식

논의했던 대로 다음 흐름으로 구현했습니다.

1. 사용자가 **수정할 데이터의 ID를 직접 입력**
2. 해당 레코드가 없으면 "찾을 수 없습니다" 안내 후 취소
3. 존재하면 **현재 값을 필드별로 보여주고**, 사용자가 새 값을 입력
   - **Enter만 입력하면 기존 값 유지** (변경 안 함)
   - 값을 입력하면 그 필드만 교체
4. 모든 필드에서 Enter만 눌렀다면(변경 사항 없음) 수정을 취소하고 저장하지 않음
5. 하나라도 변경되었다면 `updateFields()` 호출 → 파일에 즉시 반영

## Delete(삭제) 방식 — "안전하게 삭제"

1. ID 입력 → 존재하지 않으면 안내 후 종료
2. **삭제 대상 데이터를 화면에 먼저 출력**하여 사용자가 무엇을 지우는지 확인
3. `(y/n)` 확인을 받아 `y`/`Y`가 아니면 삭제를 취소
4. 확정된 경우에만 `removeById()` 호출 → 파일에 반영

## 콘솔 메뉴 (main.cpp)

```
1. 전체 목록 조회         → Read (전체)
2. ID로 검색              → Read (단건)
3. 이름(키워드)으로 검색   → Read (검색)
4. 새 데이터 추가          → Create
5. 데이터 수정            → Update
6. 데이터 삭제            → Delete
0. 종료
```

입력 처리는 `readLine`/`readRequiredLine`/`readOptionalInt` 헬퍼로 통일했습니다.
잘못된 ID나 빈 값 입력 시 프로그램이 종료되지 않고 안내 메시지를 출력한 뒤 메뉴로 돌아갑니다.

## 빌드 및 실행 환경 참고

- Visual Studio (MSVC v143 툴셋, C++17) 프로젝트로 구성했고, 솔루션(`PoC Sample.slnx`)에 등록해 두었습니다.
- 소스 파일이 한글 주석/문자열을 포함한 UTF-8이므로, 컴파일 시 `/utf-8` 옵션이 반드시 필요합니다 (vcxproj에 이미 반영됨). 이 옵션이 없으면 시스템 코드페이지(CP949)로 잘못 해석되어 컴파일 오류가 발생합니다.
- 실행 시 작업 디렉터리를 프로젝트 폴더(`CrudApp/`)로 맞춰야 `data/records.json` 상대 경로가 올바르게 인식됩니다. `.vcxproj.user`에 디버그 작업 디렉터리를 미리 설정해 두었습니다.
- Visual Studio에서 CrudApp 프로젝트를 시작 프로젝트로 지정하고 F5(디버그 시작)로 바로 실행할 수 있습니다.
- 실제로 cl.exe로 직접 빌드하여 Create/Read(전체·ID·키워드)/Update/Delete 전 기능을 콘솔 입력으로 시나리오 테스트하고 정상 동작을 확인했습니다.
