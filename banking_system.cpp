#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<ctime>

using namespace std;

// Colors
void Green() { cout << "\033[32m"; }
void Yellow(){ cout << "\033[33m"; }
void Blue()  { cout << "\033[34m"; }
void Magenta(){ cout << "\033[35m"; }
void Reset() { cout << "\033[0m"; }

// ---------------- TRANSACTION CLASS ----------------
class Transaction {
public:
    string type;
    double amount;
    string time;

    Transaction(string t, double a) {
        type = t;
        amount = a;

        time_t now = std::time(0);
        time = ctime(&now);
        time.pop_back();
    }

    string toString() {
        return type + " | " + to_string(amount) + " | " + time;
    }
};

// ---------------- CUSTOMER CLASS ----------------
class Customer {
public:
    string name;
    string email;
    string phone;
    string aadhar;

    void input() {
        cin.ignore();
        cout << "Name: "; getline(cin, name);
        cout << "Email: "; getline(cin, email);
        cout << "Phone: "; getline(cin, phone);
        cout << "Aadhar: "; getline(cin, aadhar);
    }
};

// ---------------- ACCOUNT CLASS ----------------
class Account {
private:
    string bankName = "BANKOFHK";
    int bankCode = 1000;

public:
    Customer customer;
    string accountNumber;
    string password;
    double balance = 0;
    vector<Transaction> history;

    // SAVE
    void save() {
        ofstream file(accountNumber + ".txt");

        file << customer.name << "\n"
             << customer.email << "\n"
             << customer.phone << "\n"
             << customer.aadhar << "\n"
             << accountNumber << "\n"
             << password << "\n"
             << balance << "\n";

        for (auto &t : history)
            file << t.toString() << "\n";

        file.close();
    }

    // LOAD
    void load() {
        history.clear();

        ifstream file(accountNumber + ".txt");
        if (!file) {
            cout << "Account not found!\n";
            return;
        }

        getline(file, customer.name);
        getline(file, customer.email);
        getline(file, customer.phone);
        getline(file, customer.aadhar);
        getline(file, accountNumber);
        getline(file, password);
        file >> balance;
        file.ignore();

        string line;
        while (getline(file, line)) {
            history.push_back(Transaction(line, 0));
        }

        file.close();
    }

    // CREATE ACCOUNT
    void createAccount() {
        Green();
        cout << "\n--- CREATE ACCOUNT ---\n";
        Reset();

        customer.input();

        cout << "Set Password: ";
        getline(cin, password);

        balance = 500;

        ifstream in("bankcode.txt");
        if (in) in >> bankCode;
        in.close();

        bankCode++;

        ofstream out("bankcode.txt");
        out << bankCode;
        out.close();

        accountNumber = bankName + to_string(bankCode);

        history.push_back(Transaction("Account Created", 500));

        save();

        Yellow();
        cout << "\nAccount Created!\n";
        cout << "Account No: " << accountNumber << "\n";
        cout << "Password  : " << password << "\n";
        Reset();
    }

    // LOGIN
    bool login() {
        string acc, pass;

        cin.ignore();
        cout << "Account No: ";
        getline(cin, acc);

        accountNumber = acc;

        load();

        cout << "Password: ";
        getline(cin, pass);

        if (pass == password) {
            cout << "Login Successful!\n";
            return true;
        }

        cout << "Wrong Password!\n";
        return false;
    }

    // DEPOSIT
    void deposit() {
        double amt;
        cout << "Deposit: ";
        cin >> amt;

        if (amt <= 0) {
            cout << "Invalid amount!\n";
            return;
        }

        balance += amt;
        history.push_back(Transaction("Deposit", amt));
        save();

        cout << "Done." << "\n";
        cout << "Balance: " << balance << "\n";
    }

    // WITHDRAW
    void withdraw() {
        double amt;
        cout << "Withdraw: ";
        cin >> amt;

        if (amt <= 0) {
            cout << "Invalid amount!\n";
            return;
        }
        if (amt > balance) {
            cout << "Insufficient balance!\n";
            return;
        }

        balance -= amt;
        history.push_back(Transaction("Withdraw", amt));
        save();

        cout << "Done." << "\n";
        cout << "Balance: " << balance << "\n";
    }

    // TRANSFER (🔥 NEW FEATURE)
    void transfer(Account &receiver) {
        double amt;
        cout << "Transfer Amount: ";
        cin >> amt;

        if (amt <= 0 || amt > balance) {
            cout << "Invalid!\n";
            return;
        }

        balance -= amt;
        receiver.balance += amt;

        history.push_back(Transaction("Sent Transfer", amt));
        receiver.history.push_back(Transaction("Received Transfer", amt));

        save();
        receiver.save();

        cout << "Transfer Successful!\n";
    }

    // DETAILS
    void details() {
        Magenta();
        cout << "\n--- ACCOUNT DETAILS ---\n";
        Reset();

        cout << "Name: " << customer.name << "\n";
        cout << "Account: " << accountNumber << "\n";
        cout << "Email: " << customer.email << "\n";
        cout << "Phone: " << customer.phone << "\n";
        cout << "Balance: " << balance << "\n";
    }

    // HISTORY
    void showHistory() {
        Yellow();
        cout << "\n--- TRANSACTIONS ---\n";
        Reset();

        for (auto &t : history)
            cout << t.toString() << "\n";
    }
};

// ---------------- MAIN ----------------
int main() {
    Account user, receiver;
    int choice;

    do {
        cout << "\n1.Create\n2.Login\n3.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            user.createAccount();
        }
        else if (choice == 2 && user.login()) {

            int c;
            do {
                cout << "\n1.Deposit\n2.Withdraw\n3.Transfer\n4.Details\n5.History\n6.Logout\nChoice: ";
                cin >> c;

                switch (c) {
                    case 1: user.deposit(); break;
                    case 2: user.withdraw(); break;

                    case 3:
                        cout << "Enter receiver account number: ";
                        cin >> receiver.accountNumber;
                        receiver.load();
                        user.transfer(receiver);
                        break;

                    case 4: user.details(); break;
                    case 5: user.showHistory(); break;
                    case 6: cout << "Logout...\n"; break;
                }

            } while (c != 6);
        }

    } while (choice != 3);

    return 0;
}