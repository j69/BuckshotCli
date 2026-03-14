#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

struct Player {
    int hp = 3;

    bool saw = true;
    bool cigarette = true;
    bool magnifier = true;
    bool handcuffs = true;
    bool beer = true;
    bool adrenaline = true;
    bool phone = true;
    bool pills = true;
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

bool shoot(Player& target, vector<string>& mag, bool doubleDamage)
{
    string bullet = mag.front();
    mag.erase(mag.begin());

    slowPrint("Shot fired...");
    pause(600);

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

void resetPlayer(Player& p)
{
    p.hp = 3;
    p.saw = true;
    p.cigarette = true;
    p.magnifier = true;
    p.handcuffs = true;
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

        vector<string> magazine = reloadMagazine(gen);

        bool playerTurn = true;
        bool dealerSkip = false;

        slowPrint("\nStarting round " + to_string(round));

        while (player.hp > 0 && dealer.hp > 0)
        {
            if (magazine.empty())
                magazine = reloadMagazine(gen);

            drawUI(player.hp, dealer.hp, magazine.size(), money, round);

            if (playerTurn)
            {
                cout << "1 - Shoot dealer\n";
                cout << "2 - Shoot yourself\n";
                cout << "3 - Use item\n";

                int choice;
                cin >> choice;

                if (choice == 3)
                {
                    cout << "Items:\n";
                    if (player.saw) cout << "1 Saw\n";
                    if (player.cigarette) cout << "2 Cigarette\n";
                    if (player.magnifier) cout << "3 Magnifier\n";
                    if (player.handcuffs) cout << "4 Handcuffs\n";

                    int item;
                    cin >> item;

                    if (item == 1 && player.saw)
                    {
                        player.saw = false;
                        shoot(dealer, magazine, true);
                    }

                    else if (item == 2 && player.cigarette)
                    {
                        player.cigarette = false;
                        player.hp++;
                        slowPrint("HP +1");
                        continue;
                    }

                    else if (item == 3 && player.magnifier)
                    {
                        player.magnifier = false;
                        cout << "Next shell: " << magazine.front() << endl;
                        continue;
                    }

                    else if (item == 4 && player.handcuffs)
                    {
                        player.handcuffs = false;
                        dealerSkip = true;
                        slowPrint("Dealer restrained.");
                        continue;
                    }

                    else
                    {
                        slowPrint("Item unavailable.");
                        continue;
                    }
                }

                else if (choice == 1)
                {
                    shoot(dealer, magazine, false);
                }

                else if (choice == 2)
                {
                    shoot(player, magazine, false);
                }

                else
                {
                    slowPrint("Invalid input.");
                    continue;
                }
            }

            else
            {
                if (dealerSkip)
                {
                    slowPrint("Dealer skips turn.");
                    dealerSkip = false;
                }
                else
                {
                    slowPrint("Dealer thinking...");
                    pause(800);

                    bool shootPlayer = true;

                    if (magazine.front() == "BLANK")
                        shootPlayer = false;

                    if (shootPlayer)
                        shoot(player, magazine, false);
                    else
                        shoot(dealer, magazine, false);
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

        round++;

        cout << "Current money: " << money << endl;

        if (money <= 0)
        {
            slowPrint("\nYou are out of money.");
            slowPrint("Game over.");
            break;
        }

        slowPrint("\nNext round starting...");
        pause(1500);
    }

    return 0;
}