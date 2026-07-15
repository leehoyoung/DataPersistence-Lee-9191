# CrudApp

JSON 파일을 데이터 저장소로 사용하는 C++ 콘솔 CRUD 애플리케이션입니다.

## 주요 기능

콘솔 메뉴를 통해 다음 기능을 제공합니다.

1. 전체 목록 조회
2. ID로 검색
3. 이름(키워드)으로 검색
4. 새 데이터 추가
5. 데이터 수정
6. 데이터 삭제
0. 종료

## 데이터 모델

각 레코드(`Record`)는 다음 필드를 가집니다.

- `id`: 자동 부여되는 고유 식별자
- `name`: 이름
- `description`: 설명
- `createdAt`: 생성 시각 (`YYYY-MM-DD HH:MM:SS`)

## 데이터 저장

- 모든 레코드는 `data/records.json` 파일에 JSON 형식으로 저장됩니다.
- 프로그램 시작 시 해당 파일을 읽어 메모리에 적재하며, 파일이 없으면 빈 목록으로 시작합니다.
- 저장(`Repository::save()`)은 임시 파일에 먼저 쓰고 원본 파일로 rename하는 방식을 사용해, 저장 도중 프로그램이 중단되어도 기존 파일이 손상되지 않도록 합니다.

## 구성 파일

- `main.cpp`: 콘솔 메뉴 및 사용자 입력 처리
- `Record.h`: 데이터 모델 정의 및 JSON 변환
- `Repository.h` / `Repository.cpp`: 레코드 CRUD 로직과 파일 입출력
- `Json.h` / `Json.cpp`: 자체 구현 JSON 파서/직렬화기
- `data/records.json`: 실제 데이터 저장 파일

## 빌드 및 실행

Visual Studio에서 `CrudApp.vcxproj`를 열어 빌드 후 실행합니다.
