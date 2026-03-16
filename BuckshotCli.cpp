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
#define BLUE "\033[34m"
#define GRAY "\033[90m"

/* CONSTANTS */

const int MAX_HP = 4;
const int MAX_ITEMS = 5;

/* DELAY */

void pauseMs(int ms)
{
    this_thread::sleep_for(chrono::milliseconds(ms));
}

/* LOG */

vector<string> gameLog;

void addLog(string s)
{
    gameLog.push_back(s);

    if (gameLog.size() > 10)
        gameLog.erase(gameLog.begin());
}

void drawLog()
{
    cout << "\n" << BLUE << "---- LOG ----" << RESET << endl;

    for (string& s : gameLog)
        cout << s << endl;
}

/* PLAYER */

struct Player
{
    int hp = 3;
    vector<string> inventory;
};

/* ITEM DROP BALANCE */

struct ItemDrop
{
    string name;
    int weight;
};

vector<ItemDrop> itemPool =
{
    {"Saw",10},
    {"Cigarette",20},
    {"Magnifier",15},
    {"Beer",15},
    {"Handcuffs",10},
    {"Phone",5},
    {"Pills",10},
    {"Inverter",5}
};

/* RANDOM ITEM */

string randomItem(mt19937& gen)
{
    int total = 0;

    for (auto& i : itemPool)
        total += i.weight;

    uniform_int_distribution<> dist(1, total);

    int r = dist(gen);

    int sum = 0;

    for (auto& i : itemPool)
    {
        sum += i.weight;

        if (r <= sum)
            return i.name;
    }

    return "Cigarette";
}

/* GIVE ITEMS */

void giveRandomItems(Player& p, mt19937& gen)
{
    uniform_int_distribution<> countDist(2, 4);

    int count = countDist(gen);

    for (int i = 0; i < count; i++)
    {
        if (p.inventory.size() < MAX_ITEMS)
            p.inventory.push_back(randomItem(gen));
    }
}

/* MAGAZINE */

vector<string> reloadMagazine(mt19937& gen)
{
    vector<string> mag;

    for (int i = 0; i < 3; i++)
        mag.push_back("LIVE");

    for (int i = 0; i < 4; i++)
        mag.push_back("BLANK");

    shuffle(mag.begin(), mag.end(), gen);

    addLog("Dealer loaded shotgun");

    return mag;
}

/* DRAW MAGAZINE */

void drawMagazine(int size)
{
    cout << "\nShells: [ ";

    for (int i = 0; i < size; i++)
        cout << "? ";

    cout << "]\n";
}

/* DEALER MEMORY */

vector<string> dealerMemory;

void initDealerMemory(int size)
{
    dealerMemory.assign(size, "?");
}

void rememberShell(int index, string value)
{
    if (index < dealerMemory.size())
        dealerMemory[index] = value;
}

void shiftDealerMemory()
{
    if (!dealerMemory.empty())
        dealerMemory.erase(dealerMemory.begin());
}

/* SHOOT */

bool shoot(Player& target, vector<string>& mag, bool& doubleDamage)
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
    }
    else
    {
        addLog("Blank round");
    }

    doubleDamage = false;

    shiftDealerMemory();

    return bullet == "LIVE";
}

/* ITEMS */

void useItem(Player& player,
    string item,
    vector<string>& mag,
    mt19937& gen,
    bool& doubleDamage,
    bool& skip)
{

    if (item == "Cigarette")
    {
        player.hp = min(player.hp + 1, MAX_HP);
        addLog("Cigarette +1 HP");
    }

    if (item == "Magnifier")
    {
        if (!mag.empty())
        {
            cout << "Next shell: " << mag.front() << endl;

            rememberShell(0, mag.front());

            addLog("Magnifier used");
        }
    }

    if (item == "Beer")
    {
        if (!mag.empty())
        {
            addLog("Beer removed shell");

            mag.erase(mag.begin());

            shiftDealerMemory();
        }
    }

    if (item == "Saw")
    {
        doubleDamage = true;
        addLog("Saw: double damage ready");
    }

    if (item == "Handcuffs")
    {
        skip = true;
        addLog("Dealer restrained");
    }

    if (item == "Phone")
    {
        cout << "Next shells: ";

        for (int i = 0; i < 2 && i < mag.size(); i++)
        {
            cout << mag[i] << " ";

            rememberShell(i, mag[i]);
        }

        cout << endl;

        addLog("Phone used");
    }

    if (item == "Pills")
    {
        uniform_int_distribution<> dist(0, 1);

        if (dist(gen))
        {
            player.hp = min(player.hp + 2, MAX_HP);
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

            rememberShell(0, mag.front());

            addLog("Shell inverted");
        }
    }

}

/* DEALER AI */

bool dealerShouldShootPlayer(vector<string>& mag)
{
    if (!dealerMemory.empty())
    {
        if (dealerMemory[0] == "LIVE")
            return true;

        if (dealerMemory[0] == "BLANK")
            return false;
    }

    int live = 0;
    int blank = 0;

    for (string s : mag)
    {
        if (s == "LIVE") live++;
        else blank++;
    }

    double chance = (double)live / (live + blank);

    if (chance > 0.65)
        return true;

    if (chance < 0.35)
        return false;

    return rand() % 2;
}

/* DEALER ITEM USE */

void dealerUseItem(Player& dealer,
    vector<string>& mag,
    mt19937& gen,
    bool& doubleDamage)
{
    for (int i = 0; i < dealer.inventory.size(); i++)
    {
        string item = dealer.inventory[i];

        if (item == "Magnifier")
        {
            rememberShell(0, mag.front());

            addLog("Dealer used Magnifier");

            dealer.inventory.erase(dealer.inventory.begin() + i);

            return;
        }

        if (item == "Saw")
        {
            doubleDamage = true;

            addLog("Dealer used Saw");

            dealer.inventory.erase(dealer.inventory.begin() + i);

            return;
        }
    }
}

/* MAIN */

int main()
{

    random_device rd;
    mt19937 gen(rd());

    int money = 300;
    int round = 1;

    while (money > 0)
    {

        cout << "\nMoney: " << money << "\n";
        cout << "Place bet: ";

        int bet;
        cin >> bet;

        if (bet > money)
            bet = money;

        Player player;
        Player dealer;

        giveRandomItems(player, gen);
        giveRandomItems(dealer, gen);

        bool doubleDamage = false;
        bool dealerSkip = false;

        vector<string> magazine = reloadMagazine(gen);

        initDealerMemory(magazine.size());

        bool playerTurn = true;

        while (player.hp > 0 && dealer.hp > 0 && !magazine.empty())
        {

            cout << "\n========== ROUND " << round << " ==========\n";

            cout << "Player HP " << player.hp << "\n";
            cout << "Dealer HP " << dealer.hp << "\n";

            drawMagazine(magazine.size());

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

                    dealerUseItem(dealer, magazine, gen, doubleDamage);

                    bool attack = dealerShouldShootPlayer(magazine);

                    if (attack)
                    {
                        addLog("Dealer shoots player");

                        shoot(player, magazine, doubleDamage);
                    }
                    else
                    {
                        addLog("Dealer shoots himself");

                        shoot(dealer, magazine, doubleDamage);
                    }

                }

            }

            playerTurn = !playerTurn;

        }

        if (player.hp > 0)
        {
            cout << GREEN << "You win round\n" << RESET;
            money += bet;
        }
        else
        {
            cout << RED << "Dealer wins round\n" << RESET;
            money -= bet;
        }

        round++;

    }

    cout << RED << "GAME OVER\n" << RESET;

}
