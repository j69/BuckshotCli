#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

struct Player
{
    int hp = 3;
    vector<string> inventory;
};

vector<string> allItems =
{
    "Saw",
    "Cigarette",
    "Magnifier",
    "Handcuffs",
    "Beer",
    "Phone",
    "Pills"
};

void pause(int ms)
{
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void slowPrint(string text)
{
    for (char c : text)
    {
        cout << c << flush;
        pause(10);
    }
    cout << endl;
}

void drawUI(int playerHP, int dealerHP, int shells, int money, int round)
{
    cout << "\n---------------------------------\n";
    cout << "        BUCKSHOT ROULETTE        \n";
    cout << "---------------------------------\n";
    cout << "Round     : " << round << endl;
    cout << "Money     : " << money << endl;
    cout << "Player HP : " << playerHP << endl;
    cout << "Dealer HP : " << dealerHP << endl;
    cout << "Shells    : " << shells << endl;
    cout << "---------------------------------\n";
}

vector<string> reloadMagazine(mt19937& gen)
{
    uniform_int_distribution<> dist(1, 3);

    int live = dist(gen);
    int blank = dist(gen);

    vector<string> mag;

    mag.insert(mag.end(), live, "LIVE");
    mag.insert(mag.end(), blank, "BLANK");

    shuffle(mag.begin(), mag.end(), gen);

    slowPrint("Dealer loads the shotgun...");
    pause(700);

    slowPrint("Magazine ready.");

    return mag;
}

void giveRandomItems(Player& p, mt19937& gen)
{
    uniform_int_distribution<> countDist(2, 4);
    uniform_int_distribution<> itemDist(0, allItems.size() - 1);

    int count = countDist(gen);

    for (int i = 0; i < count; i++)
    {
        string item = allItems[itemDist(gen)];
        p.inventory.push_back(item);
    }
}

void showInventory(Player& p)
{
    cout << "\nInventory:\n";

    for (int i = 0; i < p.inventory.size(); i++)
    {
        cout << i + 1 << " - " << p.inventory[i] << endl;
    }
}

void countShells(vector<string>& mag, int& live, int& blank)
{
    live = 0;
    blank = 0;

    for (string s : mag)
    {
        if (s == "LIVE") live++;
        else blank++;
    }
}

bool dealerShouldShootPlayer(vector<string>& mag, int dealerHP, int playerHP, mt19937& gen)
{
    int live, blank;
    countShells(mag, live, blank);

    int total = live + blank;

    double chance = (double)live / total;

    if (chance > 0.6)
        return true;

    if (chance < 0.4)
        return false;

    if (dealerHP <= 1)
        return true;

    uniform_int_distribution<> dist(0, 1);
    return dist(gen);
}

bool shoot(Player& target, vector<string>& mag, bool doubleDamage)
{
    string bullet = mag.front();
    mag.erase(mag.begin());

    slowPrint("Shot fired...");
    pause(500);

    if (bullet == "LIVE")
    {
        int dmg = doubleDamage ? 2 : 1;

        target.hp -= dmg;

        cout << "LIVE round. Damage " << dmg << endl;

        return true;
    }
    else
    {
        cout << "Blank round\n";
        return false;
    }
}

void useItem(Player& player, Player& dealer, vector<string>& magazine, int index, mt19937& gen, bool& doubleDamage, bool& skipDealer)
{
    string item = player.inventory[index];

    cout << "Using " << item << endl;

    if (item == "Saw")
    {
        doubleDamage = true;
    }

    else if (item == "Cigarette")
    {
        player.hp++;
        cout << "HP +1\n";
    }

    else if (item == "Magnifier")
    {
        cout << "Next shell: " << magazine.front() << endl;
    }

    else if (item == "Handcuffs")
    {
        skipDealer = true;
        cout << "Dealer restrained\n";
    }

    else if (item == "Beer")
    {
        cout << "Removed shell: " << magazine.front() << endl;
        magazine.erase(magazine.begin());
    }

    else if (item == "Phone")
    {
        uniform_int_distribution<> dist(0, magazine.size() - 1);
        int r = dist(gen);

        cout << "Phone reveals shell " << r + 1 << " : " << magazine[r] << endl;
    }

    else if (item == "Pills")
    {
        uniform_int_distribution<> dist(0, 1);

        if (dist(gen))
        {
            player.hp += 2;
            cout << "Pills worked +2 HP\n";
        }
        else
        {
            player.hp -= 1;
            cout << "Bad reaction -1 HP\n";
        }
    }

    player.inventory.erase(player.inventory.begin() + index);
}

int main()
{
    random_device rd;
    mt19937 gen(rd());

    int money = 200;
    int round = 1;

    slowPrint("Welcome to Buckshot Roulette.");

    while (money > 0)
    {
        Player player;
        Player dealer;

        giveRandomItems(player, gen);

        vector<string> magazine = reloadMagazine(gen);

        bool playerTurn = true;
        bool skipDealer = false;
        bool doubleDamage = false;

        slowPrint("\nStarting round " + to_string(round));

        while (player.hp > 0 && dealer.hp > 0)
        {
            if (magazine.empty())
                magazine = reloadMagazine(gen);

            drawUI(player.hp, dealer.hp, magazine.size(), money, round);

            if (playerTurn)
            {
                cout << "1 Shoot dealer\n";
                cout << "2 Shoot yourself\n";
                cout << "3 Use item\n";

                int choice;
                cin >> choice;

                if (choice == 3)
                {
                    if (player.inventory.empty())
                    {
                        cout << "No items\n";
                        continue;
                    }

                    showInventory(player);

                    int item;
                    cin >> item;

                    useItem(player, dealer, magazine, item - 1, gen, doubleDamage, skipDealer);

                    continue;
                }

                else if (choice == 1)
                {
                    shoot(dealer, magazine, doubleDamage);
                }

                else if (choice == 2)
                {
                    shoot(player, magazine, doubleDamage);
                }

                else
                {
                    cout << "Invalid\n";
                    continue;
                }

                doubleDamage = false;
            }

            else
            {
                if (skipDealer)
                {
                    slowPrint("Dealer skips turn.");
                    skipDealer = false;
                }
                else
                {
                    slowPrint("Dealer thinking...");
                    pause(800);

                    bool attack = dealerShouldShootPlayer(
                        magazine,
                        dealer.hp,
                        player.hp,
                        gen
                    );

                    if (attack)
                    {
                        slowPrint("Dealer aims at you.");
                        shoot(player, magazine, false);
                    }
                    else
                    {
                        slowPrint("Dealer aims at himself.");
                        shoot(dealer, magazine, false);
                    }
                }
            }

            playerTurn = !playerTurn;
        }

        if (player.hp > 0)
        {
            slowPrint("Round won.");
            money += 100;
        }
        else
        {
            slowPrint("Round lost.");
            money -= 50;
        }

        cout << "Money: " << money << endl;

        round++;

        if (money <= 0)
        {
            slowPrint("You are out of money.");
            slowPrint("Game over.");
            break;
        }

        slowPrint("Next round starting...");
        pause(1200);
    }

    return 0;
}