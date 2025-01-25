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

// Функция сравнения
bool compareByLocale(const std::string& a, const std::string& b) {
    //if (a.compare(b)) return true;
    const auto data_a = a.data();
    const auto data_b = b.data();
    const auto len_a = a.length();
    const auto len_b = b.length();
    const auto min_len = std::min(len_a, len_b);

    for (auto i = 0; i < min_len; i++){
        if (data_a[i] < data_b[i]) {
            return true;
        }
        else if (data_a[i] > data_b[i]) {
            return false;
        }
    }
    // Если префиксы совпадают, то коротка строка считается меньшей
    return len_a < len_b;
}

// Функции сравнения для Person
bool compareByFirstNameLocale(const Person& a, const Person& b) {
    if (a.firstName != b.firstName) return compareByLocale(a.firstName, b.firstName);
    if (a.lastName != b.lastName) return compareByLocale(a.lastName, b.lastName);
    return a.phoneNumber < b.phoneNumber;
}

bool compareByLastNameLocale(const Person& a, const Person& b) {
    if (a.lastName != b.lastName) return compareByLocale(a.lastName, b.lastName);
    if (a.firstName != b.firstName) return compareByLocale(a.firstName, b.firstName);
    return a.phoneNumber < b.phoneNumber;
}

// Функция сравнения для номера телефона (как строка)
bool compareByPhoneNumber(const Person& a, const Person& b) {
    return a.phoneNumber < b.phoneNumber;
}

// Функция для вывода таблицы
#define RED "\033[31m"
#define RESET "\033[0m"
void printTable(const std::vector<Person>& people) {
    if (people.empty()) {
        std::wcout << L"База данных пуста." << std::endl;
        return;
    }

    int lastNameWidth = 20;
    int firstNameWidth = 20;
    int phoneWidth = 15;
    int totalWidth = lastNameWidth + firstNameWidth + phoneWidth + 6; // +6 для разделителей

    std::wcout << RED; // Начало красного цвета
    std::wcout << L"┌";
    for (int i = 0; i < totalWidth - 2; ++i) std::wcout << L"─";
    std::wcout << L"┐" << RESET << std::endl; // Верхняя рамка

    std::wcout << RED << L"│" << RESET;
    std::wcout << std::left << std::setw(lastNameWidth) << L"Фамилия" << RED << L"│" << RESET;
    std::wcout << std::left << std::setw(firstNameWidth) << L"Имя" << RED << L"│" << RESET;
    std::wcout << std::left << std::setw(phoneWidth) << L"Телефон" << RED << L"│" << RESET << std::endl;

    std::wcout << RED << L"├";
    for (int i = 0; i < totalWidth - 2; ++i) std::wcout << L"─";
    std::wcout << L"┤" << RESET << std::endl; // Разделитель заголовка

    for (const auto& person : people) {
        std::wcout << RED << L"│" << RESET;
        std::wcout << std::left << std::setw(lastNameWidth) << s2ws(person.lastName) << RED << L"│" << RESET;
        std::wcout << std::left << std::setw(firstNameWidth) << s2ws(person.firstName) << RED << L"│" << RESET;
        std::wcout << std::left << std::setw(phoneWidth) << s2ws(person.phoneNumber) << RED << L"│" << RESET << std::endl;
    }

    std::wcout << RED << L"└";
    for (int i = 0; i < totalWidth - 2; ++i) std::wcout << L"─";
    std::wcout << L"┘" << RESET << std::endl; // Нижняя рамка
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
    std::locale::global(std::locale("")); // Устанавливаем локаль пользователя по умолчанию
    std::locale loc = std::locale(""); // Создаем объект локали

    std::wcout.imbue(loc);
    std::wcin.imbue(loc);
    std::wcerr.imbue(loc);

    std::vector<Person> people = readFromFile("input.txt");
    if (people.empty()) {
        return 1;
    }

    int choice;
    bool continueWork = true;

    while (continueWork) {
        std::wcout << L"Выберите действие:" << std::endl;
        std::wcout << L"1 - Сортировать по фамилии" << std::endl;
        std::wcout << L"2 - Сортировать по имени" << std::endl;
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
            std::sort(people.begin(), people.end(), [&](const Person& a, const Person& b) {
                return compareByFirstNameLocale(a, b);
                });
            printTable(people);
            break;
        case 2:
            std::sort(people.begin(), people.end(), [&](const Person& a, const Person& b) {
                return compareByLastNameLocale(a, b);
                });
            printTable(people);
            break;
        case 3:
            std::sort(people.begin(), people.end(), compareByPhoneNumber); // Теперь функция определена
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