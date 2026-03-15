#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <thread>
#include <chrono>

using namespace std;

/* COLORS */

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define GRAY "\033[90m"
#define BLUE "\033[34m"

/* PAUSE */

void pauseMs(int ms)
{
    this_thread::sleep_for(chrono::milliseconds(ms));
}

/* LOG */

vector<string> gameLog;
const int LOG_SIZE = 10;

void addLog(string s)
{
    gameLog.push_back(s);
    if (gameLog.size() > LOG_SIZE)
        gameLog.erase(gameLog.begin());
}

void drawLog()
{
    cout << "\n" << BLUE << "------ Game Log ------" << RESET << "\n";
    for (string& s : gameLog)
        cout << s << endl;
}

/* DEALER LINES */

vector<string> dealerLines =
{
"Dealer: Let's see how lucky you are...",
"Dealer: Your move.",
"Dealer: Interesting...",
"Dealer: This could hurt.",
"Dealer: Luck is a funny thing."
};

void dealerSpeak(mt19937& gen)
{
    uniform_int_distribution<> dist(0, dealerLines.size() - 1);
    cout << YELLOW << dealerLines[dist(gen)] << RESET << endl;
}

/* PLAYER */

struct Player
{
    int hp = 3;
    vector<string> inventory;
};

/* ITEMS */

vector<string> allItems =
{
"Saw",
"Cigarette",
"Magnifier",
"Handcuffs",
"Beer",
"Phone",
"Pills",
"Inverter"
};

/* INVENTORY */

void giveRandomItems(Player& p, mt19937& gen)
{
    uniform_int_distribution<> countDist(2, 4);
    uniform_int_distribution<> itemDist(0, allItems.size() - 1);

    int count = countDist(gen);

    for (int i = 0; i < count; i++)
        p.inventory.push_back(allItems[itemDist(gen)]);
}

/* MAGAZINE */

vector<string> reloadMagazine(mt19937& gen)
{
    vector<string> mag;

    for (int i = 0; i < 3; i++) mag.push_back("LIVE");
    for (int i = 0; i < 4; i++) mag.push_back("BLANK");

    shuffle(mag.begin(), mag.end(), gen);

    addLog("Dealer loaded shotgun");

    return mag;
}

/* MAGAZINE DISPLAY */

void drawMagazine(vector<string>& mag)
{
    cout << "\nShells: [ ";

    for (string& s : mag)
    {
        if (s == "LIVE")
            cout << RED << "LIVE " << RESET;
        else
            cout << GRAY << "BLANK " << RESET;
    }

    cout << "]\n";
}

/* SHOT */

bool shoot(Player& target, vector<string>& mag, bool doubleDamage)
{
    string bullet = mag.front();
    mag.erase(mag.begin());

    cout << "\nclick...\n";
    pauseMs(500);

    cout << RED << "BOOM\n" << RESET;

    if (bullet == "LIVE")
    {
        int dmg = doubleDamage ? 2 : 1;

        target.hp -= dmg;

        addLog("LIVE round damage " + to_string(dmg));

        return true;
    }
    else
    {
        addLog("Blank round");
        return false;
    }
}

/* AI */

bool dealerAI(vector<string>& mag)
{
    int live = 0, blank = 0;

    for (string s : mag)
    {
        if (s == "LIVE") live++;
        else blank++;
    }

    double chance = (double)live / (live + blank);

    if (chance > 0.6) return true;
    if (chance < 0.4) return false;

    return rand() % 2;
}

/* ITEMS */

void useItem(Player& player, string item, vector<string>& mag, mt19937& gen, bool& doubleDamage, bool& dealerSkip)
{

    if (item == "Cigarette")
    {
        player.hp++;
        addLog("HP +1");
    }

    if (item == "Magnifier")
    {
        if (!mag.empty())
        {
            cout << "Next shell: " << mag.front() << endl;
            addLog("Magnifier used");
        }
    }

    if (item == "Beer")
    {
        if (!mag.empty())
        {
            mag.erase(mag.begin());
            addLog("Shell removed");
        }
    }

    if (item == "Saw")
    {
        doubleDamage = true;
        addLog("Next shot double damage");
    }

    if (item == "Handcuffs")
    {
        dealerSkip = true;
        addLog("Dealer restrained");
    }

    if (item == "Phone")
    {
        cout << "Next shells: ";
        for (int i = 0; i < 2 && i < mag.size(); i++)
            cout << mag[i] << " ";
        cout << endl;

        addLog("Phone revealed shells");
    }

    if (item == "Pills")
    {
        uniform_int_distribution<>dist(0, 1);

        if (dist(gen))
        {
            player.hp += 2;
            addLog("Pills healed +2");
        }
        else
        {
            player.hp--;
            addLog("Pills side effect -1");
        }
    }

    if (item == "Inverter")
    {
        if (!mag.empty())
        {
            if (mag.front() == "LIVE")
                mag.front() = "BLANK";
            else
                mag.front() = "LIVE";

            addLog("Shell inverted");
        }
    }

}

/* MAIN */

int main()
{

    random_device rd;
    mt19937 gen(rd());

    int money = 200;
    int round = 1;

    while (money > 0)
    {

        Player player;
        Player dealer;

        bool doubleDamage = false;
        bool dealerSkip = false;

        giveRandomItems(player, gen);

        vector<string>magazine = reloadMagazine(gen);

        bool playerTurn = true;

        while (player.hp > 0 && dealer.hp > 0 && !magazine.empty())
        {

            cout << "\n=============================\n";
            cout << "Round " << round << "   Money " << money << endl;

            cout << "Player HP " << player.hp << endl;
            cout << "Dealer HP " << dealer.hp << endl;

            drawMagazine(magazine);

            drawLog();

            if (playerTurn)
            {

                cout << "\n1 Shoot dealer\n";
                cout << "2 Shoot yourself\n";
                cout << "3 Use item\n";

                int choice;
                cin >> choice;

                if (choice == 1)
                    shoot(dealer, magazine, doubleDamage);

                else if (choice == 2)
                    shoot(player, magazine, doubleDamage);

                else if (choice == 3)
                {

                    cout << "\nInventory\n";

                    for (int i = 0; i < player.inventory.size(); i++)
                        cout << i + 1 << " " << player.inventory[i] << endl;

                    int id;
                    cin >> id;
                    id--;

                    if (id >= 0 && id < player.inventory.size())
                    {

                        string item = player.inventory[id];

                        useItem(player, item, magazine, gen, doubleDamage, dealerSkip);

                        player.inventory.erase(player.inventory.begin() + id);

                    }

                    continue;
                }

            }

            else
            {

                if (dealerSkip)
                {
                    dealerSkip = false;
                    addLog("Dealer skipped turn");
                }

                else
                {

                    dealerSpeak(gen);

                    bool attack = dealerAI(magazine);

                    if (attack)
                    {
                        addLog("Dealer shoots player");
                        shoot(player, magazine, false);
                    }
                    else
                    {
                        addLog("Dealer shoots himself");
                        shoot(dealer, magazine, false);
                    }

                }

            }

            doubleDamage = false;

            playerTurn = !playerTurn;

        }

        if (player.hp > 0)
        {
            cout << GREEN << "You win round\n" << RESET;
            money += 100;
        }
        else
        {
            cout << RED << "Dealer wins round\n" << RESET;
            money -= 50;
        }

        round++;

    }

    cout << RED << "GAME OVER\n" << RESET;

}