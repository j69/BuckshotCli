// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

void slowPrint(const string& text, int delay = 25) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delay));
    }
    cout << endl;
}

vector<string> createMagazine() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 3);

    int live = dist(gen);
    int blank = dist(gen);

    vector<string> magazine;
    magazine.insert(magazine.end(), live, "LIVE");
    magazine.insert(magazine.end(), blank, "BLANK");

    shuffle(magazine.begin(), magazine.end(), gen);
    return magazine;
}

bool shoot(const string& target, vector<string>& magazine) {
    if (magazine.empty()) {
        slowPrint("No Bullets left");
        return false;
    }

    string bullet = magazine.front();
    magazine.erase(magazine.begin());

    slowPrint("Shoot... (" + bullet + ")");

    if (bullet == "LIVE") {
        slowPrint("💥 " + target + " dead.");
        return true;
    }
    else {
        slowPrint("Blank shoot.");
        return false;
    }
}

int main() {
    slowPrint("Bullets Roulette CLI in C++\n");

    vector<string> magazine = createMagazine();
    bool playerAlive = true;
    bool dealerAlive = true;
    bool playerTurn = true;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> chance(0.0, 1.0);

    while (playerAlive && dealerAlive) {
        cout << "\nBullets left: " << magazine.size() << endl;

        if (playerTurn) {
            slowPrint("\n Choose your move:");
            slowPrint("1 — Shoot yourself");
            slowPrint("2 — Shoot master");

            int choice;
            cout << "> ";
            cin >> choice;

            if (choice == 1) {
                if (shoot("You", magazine))
                    playerAlive = false;
            }
            else if (choice == 2) {
                if (shoot("Master", magazine))
                    dealerAlive = false;
            }
            else {
                slowPrint(" Wrong move");
                continue;
            }
        }
        else {
            slowPrint("\n Master will now  move...");
            this_thread::sleep_for(chrono::milliseconds(800));

            bool shootPlayer = chance(gen) > 0.5;
            if (shootPlayer) {
                if (shoot("You", magazine))
                    playerAlive = false;
            }
            else {
                if (shoot("Master", magazine))
                    dealerAlive = false;
            }
        }

        playerTurn = !playerTurn;
    }

    slowPrint("\nEnd of game");
    if (playerAlive)
        slowPrint("You win!");
    else
        slowPrint("You dead!");

    return 0;
}