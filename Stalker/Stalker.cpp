#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <limits>
#include <map>

using namespace std;

struct Person {
    string firstName;
    string lastName;
    string phoneNumber;
};

// Функции преобразования string/wstring
std::wstring s2ws(const std::string& str) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr) {
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

// Функции сравнения
bool compareByFirstName(const Person& a, const Person& b) {
    if (a.firstName != b.firstName) return a.firstName < b.firstName;
    if (a.lastName != b.lastName) return a.lastName < b.lastName;
    return a.phoneNumber < b.phoneNumber;
}

bool compareByLastName(const Person& a, const Person& b) {
    if (a.lastName != b.lastName) return a.lastName < b.lastName;
    if (a.firstName != b.firstName) return a.firstName < b.firstName;
    return a.phoneNumber < b.phoneNumber;
}

bool compareByPhoneNumber(const Person& a, const Person& b) {
    return a.phoneNumber < b.phoneNumber;
}

// Функция для вывода таблицы
void printTable(const std::vector<Person>& people) {
    std::wcout << L"--------------------------------------------------" << std::endl;
    std::wcout << std::left << std::setw(20) << L"Фамилия" << std::setw(20) << L"Имя" << std::setw(15) << L"Телефон" << std::endl;
    std::wcout << L"--------------------------------------------------" << std::endl;
    for (const auto& person : people) {
        std::wcout << std::left << std::setw(20) << s2ws(person.firstName) << std::setw(20) << s2ws(person.lastName)
            << std::setw(15) << s2ws(person.phoneNumber) << std::endl;
    }
    std::wcout << L"--------------------------------------------------" << std::endl;
}


// Функция для чтения данных из файла
std::vector<Person> readFromFile(const std::string& filename) {
    std::locale utf8_locale("ru_RU.UTF-8");
    std::wifstream inputFile(filename);
    std::vector<Person> people;
    if (!inputFile.is_open()) {
        std::wcerr << L"Не удалось открыть файл " << s2ws(filename) << std::endl;
        return people; // Возвращаем пустой вектор в случае ошибки
    }
    inputFile.imbue(utf8_locale);
    wstring line;
    while (getline(inputFile, line)) {
        wstringstream ss(line);
        wstring name;
        wstring phoneNumber;
        getline(ss, name, L':');
        ss >> phoneNumber;
        size_t spacePos = name.find(L' ');
        if (spacePos != wstring::npos) {
            Person person;
            person.firstName = ws2s(name.substr(0, spacePos));
            person.lastName = ws2s(name.substr(spacePos + 1));
            person.phoneNumber = ws2s(phoneNumber);
            people.push_back(person);
        }
        else {
            std::wcerr << L"Некорректный формат строки: " << line << std::endl;
        }
    }
    inputFile.close();
    return people;
}

int main() {
    std::locale::global(std::locale("ru_RU.UTF-8"));
    std::wcout.imbue(std::locale("ru_RU.UTF-8"));
    std::wcin.imbue(std::locale("ru_RU.UTF-8"));
    std::wcerr.imbue(std::locale("ru_RU.UTF-8"));

    std::vector<Person> people = readFromFile("input.txt");
    if (people.empty()) {
        return 1; // Завершаем программу, если не удалось прочитать файл
    }

    int choice;
    bool continueWork = true;

    while (continueWork) {
        std::wcout << L"Выберите действие:" << std::endl;
        std::wcout << L"1 - Сортировать по имени" << std::endl;
        std::wcout << L"2 - Сортировать по фамилии" << std::endl;
        std::wcout << L"3 - Сортировать по телефону" << std::endl;
        std::wcout << L"0 - Выход" << std::endl;
        std::wcout << L"Ввод: ";

        if (!(std::wcin >> choice)) {
            std::wcin.clear();
            std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::wcerr << L"Некорректный ввод. Введите число." << std::endl;
            continue;
        }

        switch (choice) {
        case 1:
            sort(people.begin(), people.end(), compareByFirstName);
            printTable(people);
            break;
        case 2:
            sort(people.begin(), people.end(), compareByLastName);
            printTable(people);
            break;
        case 3:
            sort(people.begin(), people.end(), compareByPhoneNumber);
            printTable(people);
            break;
        case 0:
            continueWork = false;
            std::wcout << L"Завершение работы." << std::endl;
            break;
        default:
            std::wcerr << L"Некорректный ввод." << std::endl;
        }
    }

    return 0;
}