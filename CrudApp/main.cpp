#include <iostream>
#include <string>
#include <optional>
#include <limits>
#include "Repository.h"

namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return trim(line);
}

// 빈 값을 허용하지 않는 문자열 입력
std::string readRequiredLine(const std::string& prompt) {
    while (true) {
        std::string line = readLine(prompt);
        if (!line.empty()) return line;
        std::cout << "값을 입력해야 합니다. 다시 입력해주세요.\n";
    }
}

std::optional<int> readOptionalInt(const std::string& prompt) {
    std::string line = readLine(prompt);
    if (line.empty()) return std::nullopt;
    try {
        return std::stoi(line);
    } catch (...) {
        return std::nullopt;
    }
}

void printRecord(const Record& r) {
    std::cout << "----------------------------------------\n";
    std::cout << "ID          : " << r.id << "\n";
    std::cout << "Name        : " << r.name << "\n";
    std::cout << "Description : " << r.description << "\n";
    std::cout << "CreatedAt   : " << r.createdAt << "\n";
}

void printRecordListHeader() {
    std::cout << "----------------------------------------\n";
}

void printRecordList(const std::vector<Record>& records) {
    if (records.empty()) {
        std::cout << "표시할 데이터가 없습니다.\n";
        return;
    }
    printRecordListHeader();
    for (const auto& r : records) {
        std::cout << "[" << r.id << "] " << r.name
                   << " | " << r.description
                   << " | " << r.createdAt << "\n";
    }
    printRecordListHeader();
    std::cout << "총 " << records.size() << "건\n";
}

void handleCreate(RecordRepository& repo) {
    std::cout << "\n=== 데이터 추가 ===\n";
    std::string name = readRequiredLine("이름: ");
    std::string description = readLine("설명: ");
    Record r = repo.create(name, description);
    std::cout << "생성되었습니다.\n";
    printRecord(r);
}

void handleListAll(RecordRepository& repo) {
    std::cout << "\n=== 전체 목록 ===\n";
    printRecordList(repo.listAll());
}

void handleFindById(RecordRepository& repo) {
    std::cout << "\n=== ID로 검색 ===\n";
    auto id = readOptionalInt("검색할 ID: ");
    if (!id.has_value()) {
        std::cout << "올바른 ID를 입력해주세요.\n";
        return;
    }
    const Record* r = repo.findById(*id);
    if (!r) {
        std::cout << "ID " << *id << " 에 해당하는 데이터를 찾을 수 없습니다.\n";
        return;
    }
    printRecord(*r);
}

void handleSearchByKeyword(RecordRepository& repo) {
    std::cout << "\n=== 이름(키워드)으로 검색 ===\n";
    std::string keyword = readRequiredLine("검색어: ");
    auto results = repo.searchByNameKeyword(keyword);
    printRecordList(results);
}

void handleUpdate(RecordRepository& repo) {
    std::cout << "\n=== 데이터 수정 ===\n";
    auto id = readOptionalInt("수정할 데이터의 ID: ");
    if (!id.has_value()) {
        std::cout << "올바른 ID를 입력해주세요.\n";
        return;
    }
    const Record* existing = repo.findById(*id);
    if (!existing) {
        std::cout << "ID " << *id << " 에 해당하는 데이터를 찾을 수 없습니다.\n";
        return;
    }

    std::cout << "현재 값은 다음과 같습니다. 변경하지 않으려면 Enter만 누르세요.\n";
    printRecord(*existing);

    std::string nameInput = readLine("새 Name [" + existing->name + "]: ");
    std::string descInput = readLine("새 Description [" + existing->description + "]: ");

    std::optional<std::string> newName = nameInput.empty() ? std::nullopt : std::optional<std::string>(nameInput);
    std::optional<std::string> newDesc = descInput.empty() ? std::nullopt : std::optional<std::string>(descInput);

    if (!newName.has_value() && !newDesc.has_value()) {
        std::cout << "변경 사항이 없어 수정을 취소합니다.\n";
        return;
    }

    bool ok = repo.updateFields(*id, newName, newDesc);
    if (ok) {
        std::cout << "수정되었습니다.\n";
        printRecord(*repo.findById(*id));
    } else {
        std::cout << "수정에 실패했습니다.\n";
    }
}

void handleDelete(RecordRepository& repo) {
    std::cout << "\n=== 데이터 삭제 ===\n";
    auto id = readOptionalInt("삭제할 데이터의 ID: ");
    if (!id.has_value()) {
        std::cout << "올바른 ID를 입력해주세요.\n";
        return;
    }
    const Record* existing = repo.findById(*id);
    if (!existing) {
        std::cout << "ID " << *id << " 에 해당하는 데이터를 찾을 수 없습니다.\n";
        return;
    }

    std::cout << "다음 데이터를 삭제합니다.\n";
    printRecord(*existing);
    std::string confirm = readLine("정말 삭제하시겠습니까? (y/n): ");
    if (confirm != "y" && confirm != "Y") {
        std::cout << "삭제를 취소했습니다.\n";
        return;
    }

    bool ok = repo.removeById(*id);
    std::cout << (ok ? "삭제되었습니다.\n" : "삭제에 실패했습니다.\n");
}

void printMenu() {
    std::cout << "\n========== CRUD 콘솔 애플리케이션 ==========\n";
    std::cout << "1. 전체 목록 조회\n";
    std::cout << "2. ID로 검색\n";
    std::cout << "3. 이름(키워드)으로 검색\n";
    std::cout << "4. 새 데이터 추가\n";
    std::cout << "5. 데이터 수정\n";
    std::cout << "6. 데이터 삭제\n";
    std::cout << "0. 종료\n";
    std::cout << "==============================================\n";
}

} // namespace

int main() {
    RecordRepository repo("data/records.json");
    repo.load();

    while (true) {
        printMenu();
        std::string choice = readLine("메뉴를 선택하세요: ");

        if (choice == "1") {
            handleListAll(repo);
        } else if (choice == "2") {
            handleFindById(repo);
        } else if (choice == "3") {
            handleSearchByKeyword(repo);
        } else if (choice == "4") {
            handleCreate(repo);
        } else if (choice == "5") {
            handleUpdate(repo);
        } else if (choice == "6") {
            handleDelete(repo);
        } else if (choice == "0") {
            std::cout << "프로그램을 종료합니다.\n";
            break;
        } else {
            std::cout << "올바른 메뉴 번호를 입력해주세요.\n";
        }
    }

    return 0;
}
