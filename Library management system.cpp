#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <limits>

// ============================== DATE CLASS ==============================
class Date {
private:
    int year, month, day;

    // Get days in a month (considering leap years)
    int daysInMonth(int m, int y) const {
        if (m == 2) {
            return (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0)) ? 29 : 28;
        }
        return (m == 4 || m == 6 || m == 9 || m == 11) ? 30 : 31;
    }

public:
    Date() : year(0), month(0), day(0) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    // Create Date from string "YYYY-MM-DD"
    static Date fromString(const std::string& dateStr) {
        int y, m, d;
        char dash1, dash2;
        std::stringstream ss(dateStr);
        ss >> y >> dash1 >> m >> dash2 >> d;
        return Date(y, m, d);
    }

    std::string toString() const {
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << year << "-"
           << std::setw(2) << std::setfill('0') << month << "-"
           << std::setw(2) << std::setfill('0') << day;
        return ss.str();
    }

    // Get current system date
    static Date currentDate() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        return Date(now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    }

    // Add days to date
    Date addDays(int days) const {
        Date result = *this;
        result.day += days;
        while (result.day > result.daysInMonth(result.month, result.year)) {
            result.day -= result.daysInMonth(result.month, result.year);
            result.month++;
            if (result.month > 12) {
                result.month = 1;
                result.year++;
            }
        }
        return result;
    }

    // Calculate difference in days (this - other)
    int daysDifference(const Date& other) const {
        // Simple approximation: convert to days since a reference
        auto toDays = [](const Date& d) {
            int days = d.year * 365 + d.day;
            for (int m = 1; m < d.month; m++) {
                days += (m == 2) ? ((d.year % 4 == 0 && (d.year % 100 != 0 || d.year % 400 == 0)) ? 29 : 28)
                                 : ((m == 4 || m == 9 || m == 11) ? 30 : 31);
            }
            return days;
        };
        return toDays(*this) - toDays(other);
    }

    bool isValid() const {
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > daysInMonth(month, year)) return false;
        return true;
    }

    bool operator>(const Date& other) const {
        if (year != other.year) return year > other.year;
        if (month != other.month) return month > other.month;
        return day > other.day;
    }

    bool operator==(const Date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
};

// ============================== BOOK CLASS ==============================
class Book {
private:
    std::string isbn;
    std::string title;
    std::string author;
    bool available;

public:
    Book() : available(true) {}
    Book(const std::string& isbn, const std::string& title, const std::string& author)
        : isbn(isbn), title(title), author(author), available(true) {}

    // Getters
    std::string getISBN() const { return isbn; }
    std::string getTitle() const { return title; }
    std::string getAuthor() const { return author; }
    bool isAvailable() const { return available; }

    // Setters
    void setAvailable(bool avail) { available = avail; }

    // Display
    void display() const {
        std::cout << "ISBN: " << isbn << " | Title: " << title 
                  << " | Author: " << author << " | Status: " 
                  << (available ? "Available" : "Checked Out") << std::endl;
    }

    // Serialization
    std::string toFileString() const {
        return isbn + "|" + title + "|" + author + "|" + (available ? "1" : "0");
    }

    static Book fromFileString(const std::string& line) {
        std::stringstream ss(line);
        std::string isbn, title, author, availStr;
        std::getline(ss, isbn, '|');
        std::getline(ss, title, '|');
        std::getline(ss, author, '|');
        std::getline(ss, availStr, '|');
        Book book(isbn, title, author);
        book.setAvailable(availStr == "1");
        return book;
    }
};

// ============================== BORROWER CLASS ==============================
class Borrower {
private:
    int id;
    std::string name;
    std::string phone;

public:
    Borrower() : id(0) {}
    Borrower(int id, const std::string& name, const std::string& phone)
        : id(id), name(name), phone(phone) {}

    int getID() const { return id; }
    std::string getName() const { return name; }
    std::string getPhone() const { return phone; }

    void display() const {
        std::cout << "ID: " << id << " | Name: " << name << " | Phone: " << phone << std::endl;
    }

    std::string toFileString() const {
        return std::to_string(id) + "|" + name + "|" + phone;
    }

    static Borrower fromFileString(const std::string& line) {
        std::stringstream ss(line);
        std::string idStr, name, phone;
        std::getline(ss, idStr, '|');
        std::getline(ss, name, '|');
        std::getline(ss, phone, '|');
        return Borrower(std::stoi(idStr), name, phone);
    }
};

// ============================== TRANSACTION CLASS ==============================
class Transaction {
private:
    int transactionID;
    std::string isbn;
    int borrowerID;
    Date checkoutDate;
    Date dueDate;
    Date returnDate;
    double fine;
    bool returned;

public:
    Transaction() : transactionID(0), borrowerID(0), fine(0.0), returned(false) {}

    Transaction(int id, const std::string& isbn, int borrowerID, const Date& checkout, const Date& due)
        : transactionID(id), isbn(isbn), borrowerID(borrowerID), 
          checkoutDate(checkout), dueDate(due), fine(0.0), returned(false) {}

    int getTransactionID() const { return transactionID; }
    std::string getISBN() const { return isbn; }
    int getBorrowerID() const { return borrowerID; }
    Date getCheckoutDate() const { return checkoutDate; }
    Date getDueDate() const { return dueDate; }
    Date getReturnDate() const { return returnDate; }
    double getFine() const { return fine; }
    bool isReturned() const { return returned; }

    void setReturned(const Date& retDate, double finePerDay) {
        returnDate = retDate;
        returned = true;
        if (retDate > dueDate) {
            int daysOverdue = retDate.daysDifference(dueDate);
            fine = daysOverdue * finePerDay;
        } else {
            fine = 0.0;
        }
    }

    void display() const {
        std::cout << "Transaction ID: " << transactionID 
                  << " | ISBN: " << isbn 
                  << " | Borrower ID: " << borrowerID
                  << " | Checkout: " << checkoutDate.toString()
                  << " | Due: " << dueDate.toString();
        if (returned) {
            std::cout << " | Return: " << returnDate.toString()
                      << " | Fine: $" << std::fixed << std::setprecision(2) << fine;
        } else {
            std::cout << " | Status: Active";
        }
        std::cout << std::endl;
    }

    std::string toFileString() const {
        std::stringstream ss;
        ss << transactionID << "|" << isbn << "|" << borrowerID << "|"
           << checkoutDate.toString() << "|" << dueDate.toString() << "|";
        if (returned) {
            ss << returnDate.toString() << "|" << fine << "|1";
        } else {
            ss << "0000-00-00|0|0";
        }
        return ss.str();
    }

    static Transaction fromFileString(const std::string& line) {
        std::stringstream ss(line);
        std::string tidStr, isbn, bidStr, checkoutStr, dueStr, returnStr, fineStr, returnedStr;
        std::getline(ss, tidStr, '|');
        std::getline(ss, isbn, '|');
        std::getline(ss, bidStr, '|');
        std::getline(ss, checkoutStr, '|');
        std::getline(ss, dueStr, '|');
        std::getline(ss, returnStr, '|');
        std::getline(ss, fineStr, '|');
        std::getline(ss, returnedStr, '|');
        
        Transaction t(std::stoi(tidStr), isbn, std::stoi(bidStr), 
                      Date::fromString(checkoutStr), Date::fromString(dueStr));
        if (returnedStr == "1") {
            t.returned = true;
            t.returnDate = Date::fromString(returnStr);
            t.fine = std::stod(fineStr);
        }
        return t;
    }
};

// ============================== LIBRARY SYSTEM ==============================
class LibrarySystem {
private:
    std::vector<Book> books;
    std::vector<Borrower> borrowers;
    std::vector<Transaction> transactions;
    int nextBorrowerID;
    int nextTransactionID;
    const double FINE_PER_DAY = 0.50;  // $0.50 per day late
    const int LOAN_DAYS = 14;          // 2 weeks loan period

    // File names
    const std::string BOOKS_FILE = "books.txt";
    const std::string BORROWERS_FILE = "borrowers.txt";
    const std::string TRANSACTIONS_FILE = "transactions.txt";

    // Helper: Convert string to lowercase
    std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    // Helper: Check if a string contains another (case-insensitive)
    bool containsIgnoreCase(const std::string& str, const std::string& substr) const {
        return toLower(str).find(toLower(substr)) != std::string::npos;
    }

    // Load data from files
    void loadBooks() {
        std::ifstream file(BOOKS_FILE);
        if (!file.is_open()) return;
        
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                books.push_back(Book::fromFileString(line));
            }
        }
        file.close();
    }

    void loadBorrowers() {
        std::ifstream file(BORROWERS_FILE);
        if (!file.is_open()) return;
        
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                borrowers.push_back(Borrower::fromFileString(line));
                if (borrowers.back().getID() >= nextBorrowerID) {
                    nextBorrowerID = borrowers.back().getID() + 1;
                }
            }
        }
        file.close();
    }

    void loadTransactions() {
        std::ifstream file(TRANSACTIONS_FILE);
        if (!file.is_open()) return;
        
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                transactions.push_back(Transaction::fromFileString(line));
                if (transactions.back().getTransactionID() >= nextTransactionID) {
                    nextTransactionID = transactions.back().getTransactionID() + 1;
                }
            }
        }
        file.close();
    }

    // Save data to files
    void saveBooks() const {
        std::ofstream file(BOOKS_FILE);
        for (const auto& book : books) {
            file << book.toFileString() << std::endl;
        }
        file.close();
    }

    void saveBorrowers() const {
        std::ofstream file(BORROWERS_FILE);
        for (const auto& borrower : borrowers) {
            file << borrower.toFileString() << std::endl;
        }
        file.close();
    }

    void saveTransactions() const {
        std::ofstream file(TRANSACTIONS_FILE);
        for (const auto& transaction : transactions) {
            file << transaction.toFileString() << std::endl;
        }
        file.close();
    }

    // Find book by ISBN
    int findBookIndex(const std::string& isbn) const {
        for (size_t i = 0; i < books.size(); i++) {
            if (books[i].getISBN() == isbn) {
                return i;
            }
        }
        return -1;
    }

    // Find borrower by ID
    int findBorrowerIndex(int id) const {
        for (size_t i = 0; i < borrowers.size(); i++) {
            if (borrowers[i].getID() == id) {
                return i;
            }
        }
        return -1;
    }

    // Find active transaction (not returned) for a book
    int findActiveTransactionByISBN(const std::string& isbn) const {
        for (size_t i = 0; i < transactions.size(); i++) {
            if (!transactions[i].isReturned() && transactions[i].getISBN() == isbn) {
                return i;
            }
        }
        return -1;
    }

public:
    LibrarySystem() : nextBorrowerID(1), nextTransactionID(1) {
        loadBooks();
        loadBorrowers();
        loadTransactions();
    }

    ~LibrarySystem() {
        saveBooks();
        saveBorrowers();
        saveTransactions();
    }

    // ====================== BOOK MANAGEMENT ======================
    void addBook() {
        std::string isbn, title, author;
        std::cin.ignore();
        std::cout << "Enter ISBN: ";
        std::getline(std::cin, isbn);
        
        if (findBookIndex(isbn) != -1) {
            std::cout << "Book with this ISBN already exists!\n";
            return;
        }
        
        std::cout << "Enter Title: ";
        std::getline(std::cin, title);
        std::cout << "Enter Author: ";
        std::getline(std::cin, author);
        
        books.push_back(Book(isbn, title, author));
        saveBooks();
        std::cout << "Book added successfully!\n";
    }

    void searchBooks() {
        int choice;
        std::cout << "\n--- Search Books ---\n";
        std::cout << "1. By Title\n";
        std::cout << "2. By Author\n";
        std::cout << "3. By ISBN\n";
        std::cout << "Enter choice: ";
        std::cin >> choice;
        
        std::string query;
        std::cin.ignore();
        std::cout << "Enter search term: ";
        std::getline(std::cin, query);
        
        std::vector<Book> results;
        switch (choice) {
            case 1:
                for (const auto& book : books) {
                    if (containsIgnoreCase(book.getTitle(), query)) {
                        results.push_back(book);
                    }
                }
                break;
            case 2:
                for (const auto& book : books) {
                    if (containsIgnoreCase(book.getAuthor(), query)) {
                        results.push_back(book);
                    }
                }
                break;
            case 3:
                for (const auto& book : books) {
                    if (containsIgnoreCase(book.getISBN(), query)) {
                        results.push_back(book);
                    }
                }
                break;
            default:
                std::cout << "Invalid choice!\n";
                return;
        }
        
        if (results.empty()) {
            std::cout << "No books found.\n";
        } else {
            std::cout << "\nFound " << results.size() << " book(s):\n";
            for (const auto& book : results) {
                book.display();
            }
        }
    }

    void displayAllBooks() const {
        if (books.empty()) {
            std::cout << "No books in library.\n";
            return;
        }
        std::cout << "\n--- All Books ---\n";
        for (const auto& book : books) {
            book.display();
        }
    }

    // ====================== BORROWER MANAGEMENT ======================
    void registerBorrower() {
        std::string name, phone;
        std::cin.ignore();
        std::cout << "Enter Name: ";
        std::getline(std::cin, name);
        std::cout << "Enter Phone: ";
        std::getline(std::cin, phone);
        
        Borrower newBorrower(nextBorrowerID++, name, phone);
        borrowers.push_back(newBorrower);
        saveBorrowers();
        std::cout << "Borrower registered! ID: " << newBorrower.getID() << std::endl;
    }

    void displayAllBorrowers() const {
        if (borrowers.empty()) {
            std::cout << "No borrowers registered.\n";
            return;
        }
        std::cout << "\n--- All Borrowers ---\n";
        for (const auto& borrower : borrowers) {
            borrower.display();
        }
    }

    // ====================== CHECKOUT & RETURN ======================
    void checkoutBook() {
        std::string isbn;
        int borrowerID;
        
        std::cout << "Enter Book ISBN: ";
        std::cin >> isbn;
        
        int bookIndex = findBookIndex(isbn);
        if (bookIndex == -1) {
            std::cout << "Book not found!\n";
            return;
        }
        
        if (!books[bookIndex].isAvailable()) {
            std::cout << "Book is already checked out!\n";
            return;
        }
        
        std::cout << "Enter Borrower ID: ";
        std::cin >> borrowerID;
        
        if (findBorrowerIndex(borrowerID) == -1) {
            std::cout << "Borrower not found!\n";
            return;
        }
        
        // Process checkout
        Date checkoutDate = Date::currentDate();
        Date dueDate = checkoutDate.addDays(LOAN_DAYS);
        
        Transaction newTransaction(nextTransactionID++, isbn, borrowerID, checkoutDate, dueDate);
        transactions.push_back(newTransaction);
        books[bookIndex].setAvailable(false);
        
        saveBooks();
        saveTransactions();
        
        std::cout << "Book checked out successfully!\n";
        std::cout << "Due Date: " << dueDate.toString() << std::endl;
    }

    void returnBook() {
        std::string isbn;
        std::cout << "Enter Book ISBN: ";
        std::cin >> isbn;
        
        int bookIndex = findBookIndex(isbn);
        if (bookIndex == -1) {
            std::cout << "Book not found!\n";
            return;
        }
        
        if (books[bookIndex].isAvailable()) {
            std::cout << "Book is not checked out!\n";
            return;
        }
        
        int transIndex = findActiveTransactionByISBN(isbn);
        if (transIndex == -1) {
            std::cout << "No active transaction found for this book!\n";
            return;
        }
        
        Date returnDate = Date::currentDate();
        transactions[transIndex].setReturned(returnDate, FINE_PER_DAY);
        books[bookIndex].setAvailable(true);
        
        saveBooks();
        saveTransactions();
        
        double fine = transactions[transIndex].getFine();
        std::cout << "Book returned successfully!\n";
        if (fine > 0) {
            std::cout << "Book is overdue. Fine: $" << std::fixed << std::setprecision(2) << fine << std::endl;
        } else {
            std::cout << "Book returned on time. No fine.\n";
        }
    }

    // ====================== TRANSACTIONS & FINES ======================
    void displayAllTransactions() const {
        if (transactions.empty()) {
            std::cout << "No transactions.\n";
            return;
        }
        std::cout << "\n--- All Transactions ---\n";
        for (const auto& trans : transactions) {
            trans.display();
        }
    }

    void displayActiveTransactions() const {
        bool found = false;
        std::cout << "\n--- Active Checkouts ---\n";
        for (const auto& trans : transactions) {
            if (!trans.isReturned()) {
                trans.display();
                found = true;
            }
        }
        if (!found) {
            std::cout << "No active checkouts.\n";
        }
    }

    void calculateFineForBook() {
        std::string isbn;
        std::cout << "Enter Book ISBN: ";
        std::cin >> isbn;
        
        int transIndex = findActiveTransactionByISBN(isbn);
        if (transIndex == -1) {
            std::cout << "No active checkout found for this book.\n";
            return;
        }
        
        Date currentDate = Date::currentDate();
        Date dueDate = transactions[transIndex].getDueDate();
        
        if (currentDate > dueDate) {
            int daysOverdue = currentDate.daysDifference(dueDate);
            double fine = daysOverdue * FINE_PER_DAY;
            std::cout << "Book is overdue by " << daysOverdue << " day(s).\n";
            std::cout << "Current fine: $" << std::fixed << std::setprecision(2) << fine << std::endl;
        } else {
            std::cout << "Book is not overdue.\n";
        }
    }

    // ====================== MAIN MENU ======================
    void displayMenu() const {
        std::cout << "\n========================================\n";
        std::cout << "       LIBRARY MANAGEMENT SYSTEM        \n";
        std::cout << "========================================\n";
        std::cout << "1. Add New Book\n";
        std::cout << "2. Search Books\n";
        std::cout << "3. Display All Books\n";
        std::cout << "4. Register Borrower\n";
        std::cout << "5. Display All Borrowers\n";
        std::cout << "6. Checkout Book\n";
        std::cout << "7. Return Book\n";
        std::cout << "8. View All Transactions\n";
        std::cout << "9. View Active Checkouts\n";
        std::cout << "10. Calculate Fine for a Book\n";
        std::cout << "11. Exit\n";
        std::cout << "========================================\n";
        std::cout << "Enter choice: ";
    }

    void run() {
        int choice;
        do {
            displayMenu();
            std::cin >> choice;
            
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                continue;
            }
            
            switch (choice) {
                case 1:
                    addBook();
                    break;
                case 2:
                    searchBooks();
                    break;
                case 3:
                    displayAllBooks();
                    break;
                case 4:
                    registerBorrower();
                    break;
                case 5:
                    displayAllBorrowers();
                    break;
                case 6:
                    checkoutBook();
                    break;
                case 7:
                    returnBook();
                    break;
                case 8:
                    displayAllTransactions();
                    break;
                case 9:
                    displayActiveTransactions();
                    break;
                case 10:
                    calculateFineForBook();
                    break;
                case 11:
                    std::cout << "Exiting system. Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid choice. Please try again.\n";
            }
        } while (choice != 11);
    }
};

// ============================== MAIN FUNCTION ==============================
int main() {
    LibrarySystem library;
    library.run();
    return 0;
}