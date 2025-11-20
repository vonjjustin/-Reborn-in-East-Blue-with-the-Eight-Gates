#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <limits>
#include <vector>
#include <thread>
#include <chrono>

using namespace std;

// CONSTANTS
const int MAX_SKILLS = 8;
const int TRAINING_TURNS = 48;
const int G1_CAST_COST = 35;
const int G1_TURN_COST = 7;
const int G1_CAST_COST_MASTERY = 30;
const int G1_TURN_COST_MASTERY = 5;
const int G2_CAST_COST = 45;
const int G2_TURN_COST = 9;
const int G2_SWITCH_COST = 10;
const int ARLONG_HP_MAX = 2500;
const int ARLONG_PHASE2_THRESHOLD = 1600;
const int ARLONG_PHASE3_THRESHOLD = 900;

// PLAYER
string playerName;
int playerHP_current;
int playerHP_max;
int playerATK;
int playerEND;
double playerEP_current;
double playerEP_max;
double playerEP_regen;

// ENEMY
string enemyName;
int enemyHP_current;
int enemyHP_max;
int enemyATK;
int enemyEND;
int enemyPhase = 0; // 0=Base, 1=Kiribachi, 2=Enraged
bool enemyIsStaggered = false;
bool enemyIsDisarmed = false;

// SKILL DATA ARRAYS [MAX_SKILLS]
int id[MAX_SKILLS];
string name[MAX_SKILLS];
int epCost[MAX_SKILLS];
int cooldownMax[MAX_SKILLS];
int cooldownLeft[MAX_SKILLS];
bool requiresGate[MAX_SKILLS];
bool g2Only[MAX_SKILLS];
bool requiresChains[MAX_SKILLS];
bool srequiresFrontLotusUsed[MAX_SKILLS];
int hits[MAX_SKILLS];
double atkMultiplier[MAX_SKILLS];
double endPierceFactor[MAX_SKILLS];
bool appliedStagger[MAX_SKILLS];
bool disarmsKiribachi[MAX_SKILLS];
int skillCount = 0;

// STATUS ARRAYS (MAX 5 STATUSES)
string NAM[5];
int statusTurnsLeft[5];
double statusATKDivisor[5];
double statusENDDivisor[5];
bool statusPreventAction[5];
int statusCount = 0;

// STATE VARIABLES
int gateState = 0; // 0=OFF, 1=G1, 2=G2
bool hasGateMastery = false;
bool hasChainHandling = false;
bool hasFrontLotusUsed = false;
bool gateEndDebuff = false;

// Training history [5]: HP, ATK, END, EP Pool, EP Regen, Self-Discovery
int trainingHistory[6] = {0, 0, 0, 0, 0, 0};

// FUNCTIONS DECLARATION
void InitGame();
void Border();
void PressEnterToContinue();
void Loading();
void DisplayMainMenu();
void DisplayAct1();
void DisplayAct2Intro();
void StartBattle();
void DisplayTrainingStats(int turnsLeft);
void TrainingLoop();
void ApplyStatGain(int statType);
void AddSkill(int skillId);
void BattleLoop();
void BeginTurn();
void PlayerActionMenu(bool &turnEnded);
void UseSkill(int skillIndex, bool &turnEnded);
void EnemyAction();
void EndGate();
void ApplyDamage(double atkMult, double endPierce, int &damageDealt);
void TickCooldownsAndStatuses();
void UpdateEnemyPhase();
void ShowEnding(bool win);

// MAIN FUNCTION
int main() {
    InitGame();
    DisplayMainMenu();
    
    // Act 1: The Fall of Cocoyashi
    DisplayAct1();
    
    // Act 2: Training Arc
    DisplayAct2Intro();
    TrainingLoop();
    
    // Act 3: Battle at Arlong Park
    StartBattle();

    cout << "\n\nPlay again? (1 = Yes, 0 = No): ";
    int playAgain;
    cin >> playAgain;
    
    if (playAgain == 1) {
        cin.ignore();
        main();
    } else {
        cout << "\nThank you for playing!\n";
        return 0;
    }
}

// FUNCTIONS INITIALIZATION
void InitGame() {
    playerName = "Keigan 'Kei' Lee";
    playerHP_current = 50;
    playerHP_max = 50;
    playerATK = 10;
    playerEND = 15;
    playerEP_current = 5;
    playerEP_max = 5;
    playerEP_regen = 0;
    
    enemyName = "Arlong the Saw";
    enemyHP_current = ARLONG_HP_MAX;
    enemyHP_max = ARLONG_HP_MAX;
    enemyATK = 520;
    enemyEND = 600;
    enemyPhase = 0;
    enemyIsStaggered = false;
    enemyIsDisarmed = false;
    
    skillCount = 0;
    statusCount = 0;
    gateState = 0;
    hasGateMastery = false;
    hasChainHandling = false;
    hasFrontLotusUsed = false;
    gateEndDebuff = false;
    
    for(int i = 0; i < 5; i++) {
        trainingHistory[i] = 0;
    }
}

void Border() {
    cout << "==================================================================================";
}

void PressEnterToContinue() {
    Border();
    cout << "\nPress any key to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void Loading()
{
    for (int loop = 0; loop < 2; ++loop) {
        for (int i = 1; i <= 3; ++i) {
            cout << '\r' << "Loading" << string(i, '.')
                 << string(3 - i, ' ') // clear leftover dots
                 << flush;
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    // After finishing, clear the whole line
    cout << '\r' << string(20, ' ') << '\r';  // overwrite with spaces, then return cursor
}

void DisplayMainMenu() {
    Border();
    cout << "\nKEIGAN'S JOURNEY: COCOYASHI ARC\n";
    cout << "A Tale of Revenge and Hope\n";
    Border();
    cout << "\n";
    Loading();
    cout << "\nWelcome to 'Keigan's Journey: Cocoyashi Arc'!\n\n";
    PressEnterToContinue();
    Border();
    cout << "\n1. Start\n";
    cout << "2. Quit\n";
    cout << "\nChoice: ";

    int choice;
    cin >> choice;
    cin.ignore();
    
    if (choice == 2) {
        cout << "\nThank you for playing!\n";
        exit(0); 
    }

    while(choice < 1 || choice > 2) {
            cout << "Invalid choice. Please select 1-2: ";
            cin >> choice;
            
            if (choice == 2) {
                cout << "\nThank you for playing!\n";
                exit(0); 
            }
        }
}

void DisplayAct1() {
    Border();
    cout << "\nACT 1: THE FALL OF COCOYASHI\n";
    Border();
    cout << "\nCocoyashi Village falls into darkness as Arlong and his crew arrive.\n";
    cout << "Their demand is simple: pay tribute, or die.\n\n";
    cout << "Bell-mere, unable to pay for herself, pays only for her daughters.\n";
    cout << "Nami and Nojiko watch in horror as she is executed before their eyes.\n\n";
    cout << "Keigan: \"ARLONG!!!\"\n\n";
    cout << "Enraged, young Keigan lunges at the Fishman captain.\n";
    cout << "His blow connects... but it's meaningless.\n\n";
    cout << "Arlong (laughing): \"Weak humans... You belong to the deep!\"\n\n";
    cout << "Arlong throws Keigan into the open sea, leaving him for dead.\n";
    cout << "But fate has other plans...\n\n";
    cout << "Days later, Keigan washes ashore on a distant island.\n";
    cout << "Battered but alive, a single purpose ignites within him:\n\n";
    cout << "Keigan (whispers): \"I will return... and I will destroy you, Arlong.\"\n\n";
    PressEnterToContinue();
}

void DisplayAct2Intro() {
    Border();
    cout << "\nACT 2: THE EIGHT-YEAR TRAINING ARC\n";
    Border();
    cout << "\nRescued by the Good Heart Pirates, Keigan is brought to a quiet island.\n\n";
    cout << "Captain: \"Live first. Then choose what to do with the life you still have.\"\n\n";
    cout << "Alone on the island, Keigan discovers an ancient scroll:\n";
    cout << "\"The Eight Inner Gates\"\n\n";
    cout << "From that day forward, training becomes his entire existence.\n";
    cout << "Every dawn brings pain. Every sunset brings progress.\n\n";
    cout << "Eight years of relentless training await...\n\n";
    PressEnterToContinue();
}

void StartBattle() {
    Border();
    cout << "\nACT 3: RETURN TO COCOYASHI\n";
    Border();
    cout << "\nEight years have passed. Keigan's transformation is complete.\n\n";
    cout << "At Baratie, fate brings him face to face with Luffy and the Straw Hats.\n";
    cout << "When he learns Nami has returned to Arlong Park, everything clicks.\n\n";
    cout << "Nojiko: \"There was a boy once... Keigan. Arlong threw him into the sea.\"\n";
    cout << "Luffy (grinning): \"So you're here to help us!\"\n";
    cout << "Keigan (quietly): \"I promised I would come back. I'm here to end it.\"\n\n";
    cout << "Together, they march toward Arlong Park.\n";
    cout << "The gates loom like a promise and a threat.\n\n";
    cout << "Keigan (under his breath): \"This time I bring what I should have had eight years ago.\"\n\n";
    PressEnterToContinue();
    
    Border();
    cout << "\nThe crew bursts through the gates of Arlong Park.\n\n";
    cout << "Arlong (sneering): \"More humans? Come to die?\"\n";
    cout << "Arlong (looking at Keigan): \"Wait... you. The weakling I threw in the sea?\"\n";
    cout << "Arlong (laughing): \"You survived? Impressive. But you're still just a human!\"\n\n";
    cout << "Keigan: \"I'm not the same boy you threw away. This ends today.\"\n\n";
    cout << "Luffy: \"Keigan! Show him what eight years of training can do!\"\n\n";
    cout << "THE BATTLE BEGINS!\n\n";
    PressEnterToContinue();

    BattleLoop();
}

void DisplayTrainingStats(int turnsLeft) {
    Border();
    cout << "\nTRAINING PROGRESS\n";
    cout << "Turns Remaining: " << turnsLeft << " / " << TRAINING_TURNS << "\n\n";
    cout << "Current Stats:\n";
    cout << "  HP: " << playerHP_max << "\n";
    cout << "  ATK: " << playerATK << "\n";
    cout << "  END: " << playerEND << "\n";
    cout << "  EP Pool: " << fixed << setprecision(1) << playerEP_max << "\n";
    cout << "  EP Regen: " << playerEP_regen << "\n\n";
    
    cout << "Training History:\n";
    cout << "  HP Sessions: " << trainingHistory[0] << "\n";
    cout << "  ATK Sessions: " << trainingHistory[1] << "\n";
    cout << "  END Sessions: " << trainingHistory[2] << "\n";
    cout << "  EP Pool Sessions: " << trainingHistory[3] << "\n";
    cout << "  EP Regen Sessions: " << trainingHistory[4] << "\n";
    cout << "  Self-Discovery: " << trainingHistory[5] << "\n";
    Border();
}

void TrainingLoop() {
    for(int turn = 1; turn <= TRAINING_TURNS; turn++) {
        int turnsLeft = TRAINING_TURNS - turn + 1;
        DisplayTrainingStats(turnsLeft);
        cout << "\nYear " << ((turn - 1) / 6 + 1) << ", Month " << (((turn - 1) % 6) * 2 + 1) << "-" << (((turn - 1) % 6) * 2 + 2) << endl;
        
        cout << "Choose your training focus:\n";
        cout << "1. HP Training\n";
        cout << "2. ATK Training\n";
        cout << "3. END Training\n";
        cout << "4. EP Pool Training\n";
        cout << "5. EP Regen Training\n";

        bool allowSD = (trainingHistory[5] < 8);

        if (allowSD) {
            cout << "6. Self-Discovery (Unlock Skills)\n";
        }

        cout << "Choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        while (true) {
            if (choice >= 1 && choice <= 5) break;
            if (choice == 6 && allowSD) break;

            cout << "Invalid choice. Please select again: ";
            cin >> choice;
            cin.ignore();
        }

        ApplyStatGain(choice);
        
        cout << "\nTraining session complete!\n";
        PressEnterToContinue();
    }
    
    Border();
    cout << "\n\nTRAINING COMPLETE\n\n";
    Border();
    cout << "\nEight years have passed...\n";
    cout << "Keigan has transformed from a broken boy into a warrior.\n\n";
    cout << "Final Stats:\n";
    cout << "  HP: " << playerHP_max << "\n";
    cout << "  ATK: " << playerATK << "\n";
    cout << "  END: " << playerEND << "\n";
    cout << "  EP Pool: " << fixed << setprecision(1) << playerEP_max << "\n";
    cout << "  EP Regen: " << playerEP_regen << "\n";
    cout << "  Skills Unlocked: " << skillCount << "\n";
    
    playerHP_current = playerHP_max;
    playerEP_current = playerEP_max;
    
    PressEnterToContinue();
}

void ApplyStatGain(int statType) {
    int n = trainingHistory[statType - 1];
    
    switch(statType) {
        case 1: { // HP
            int gain = 21 + 4 * n;
            playerHP_max += gain;
            trainingHistory[0]++;
            cout << "\n[HP increased by " << gain << "!]";
            break;
        }
        case 2: { // ATK
            int gain = 11 + 2 * n;
            playerATK += gain;
            trainingHistory[1]++;
            cout << "\n[ATK increased by " << gain << "!]";
            break;
        }
        case 3: { // END
            int gain = 15 + 3 * n;
            playerEND += gain;
            trainingHistory[2]++;
            cout << "\n[END increased by " << gain << "!]";
            break;
        }
        case 4: { // EP Pool
            double gainPool = 15 + 3 * n;
            playerEP_max += gainPool;
            trainingHistory[3]++;
            cout << "\n[EP Pool increased by " << gainPool << "!]";
            break;
        }

        case 5: { // EP Regen
            double gainRegen = 2.5 + 0.5 * n;
            playerEP_regen += gainRegen;
            trainingHistory[4]++;
            cout << "\n[EP Regen increased by " << gainRegen << "!]";
            break;
        }

        case 6: { // Self-Discovery
            trainingHistory[5]++;
            int sdCount = trainingHistory[5];
            
            if(sdCount == 1) {
                AddSkill(1); // Gate of Opening
                cout << "\n[Unlocked: Gate of Opening (G1)!]";
            } else if(sdCount == 2) {
                AddSkill(2); // Chain Handling
                hasChainHandling = true;
                cout << "\n[Unlocked: Chain Handling!]";
            } else if(sdCount == 3) {
                AddSkill(3); // Kei-ga-n Barrage
                cout << "\n[Unlocked: Kei-ga-n Barrage!]";
            } else if(sdCount == 4) {
                AddSkill(4); // Front Lotus
                cout << "\n[Unlocked: Front Lotus (Omote Renge)!]";
            } else if(sdCount == 5) {
                hasGateMastery = true;
                cout << "\n[Unlocked: Gate Mastery!]";
            } else if(sdCount == 6) {
                AddSkill(6); // Gate of Healing
                cout << "\n[Unlocked: Gate of Healing (G2)!]";
            } else if(sdCount == 7) {
                AddSkill(7); // Chain Barrage
                cout << "\n[Unlocked: Chain Barrage!]";
            } else if(sdCount == 8) {
                AddSkill(8); // Reverse Lotus
                cout << "\n[Unlocked: Reverse Lotus (Ura Renge)!]";
            } else {
                cout << "\n[Self-Discovery session completed. Inner strength grows.]";
            }
            break;
        }
    }
}

void AddSkill(int skillId) {
    if(skillCount >= MAX_SKILLS) return;
    
    int index = skillCount;
    id[index] = skillId;
    cooldownLeft[index] = 0;
    
    switch(skillId) {
        case 1: // Gate of Opening
            name[index] = "Gate of Opening (G1)";
            epCost[index] = hasGateMastery ? G1_CAST_COST_MASTERY : G1_CAST_COST;
            cooldownMax[index] = 0;
            requiresGate[index] = false;
            g2Only[index] = false;
            requiresChains[index] = false;
            srequiresFrontLotusUsed[index] = false;
            hits[index] = 0;
            atkMultiplier[index] = 1.8;
            endPierceFactor[index] = 0;
            appliedStagger[index] = false;
            disarmsKiribachi[index] = false;
            break;
            
        case 2: // Chain Handling
            name[index] = "Chain Handling";
            epCost[index] = 0;
            cooldownMax[index] = 0;
            requiresGate[index] = false;
            g2Only[index] = false;
            requiresChains[index] = false;
            srequiresFrontLotusUsed[index] = false;
            hits[index] = 0;
            atkMultiplier[index] = 0;
            endPierceFactor[index] = 0;
            appliedStagger[index] = false;
            disarmsKiribachi[index] = false;
            break;
            
        case 3: // Kei-ga-n Barrage
            name[index] = "Kei-ga-n Barrage";
            epCost[index] = 12;
            cooldownMax[index] = 2;
            requiresGate[index] = false;
            g2Only[index] = false;
            requiresChains[index] = false;
            srequiresFrontLotusUsed[index] = false;
            hits[index] = 3;
            atkMultiplier[index] = 0.45;
            endPierceFactor[index] = 0.05;
            appliedStagger[index] = false;
            disarmsKiribachi[index] = false;
            break;
            
        case 4: // Front Lotus
            name[index] = "Front Lotus (Omote Renge)";
            epCost[index] = 13;
            cooldownMax[index] = 0;
            requiresGate[index] = true;
            g2Only[index] = false;
            requiresChains[index] = true;
            srequiresFrontLotusUsed[index] = false;
            hits[index] = 2;
            atkMultiplier[index] = 0.95;
            endPierceFactor[index] = 0.30;
            appliedStagger[index] = true;
            disarmsKiribachi[index] = true;
            break;
            
        case 6: // Gate of Healing
            name[index] = "Gate of Healing (G2)";
            epCost[index] = G2_CAST_COST;
            cooldownMax[index] = 0;
            requiresGate[index] = false;
            g2Only[index] = false;
            requiresChains[index] = false;
            srequiresFrontLotusUsed[index] = false;
            hits[index] = 0;
            atkMultiplier[index] = 2.2;
            endPierceFactor[index] = 0;
            appliedStagger[index] = false;
            disarmsKiribachi[index] = false;
            break;
            
        case 7: // Chain Barrage
            name[index] = "Chain Barrage";
            epCost[index] = 14;
            cooldownMax[index] = 2;
            requiresGate[index] = false;
            g2Only[index] = false;
            requiresChains[index] = true;
            srequiresFrontLotusUsed[index] = false;
            hits[index] = 3;
            atkMultiplier[index] = 0.5;
            endPierceFactor[index] = 0.10;
            appliedStagger[index] = false;
            disarmsKiribachi[index] = false;
            break;
            
        case 8: // Reverse Lotus
            name[index] = "Reverse Lotus (Ura Renge)";
            epCost[index] = 25;
            cooldownMax[index] = 5;
            requiresGate[index] = true;
            g2Only[index] = false;
            requiresChains[index] = false;
            srequiresFrontLotusUsed[index] = true;
            hits[index] = 4;
            atkMultiplier[index] = 1.25;
            endPierceFactor[index] = 0.35;
            appliedStagger[index] = true;
            disarmsKiribachi[index] = false;
            break;
    }
    
    skillCount++;
}

void BattleLoop() {
    int turnNumber = 0;
    
    while(playerHP_current > 0 && enemyHP_current > 0) {
        turnNumber++;
        Border();
        cout << "\n=== TURN " << turnNumber << " ===\n\n";
        
        // Display Status
        cout << "KEIGAN: HP=" << playerHP_current << "/" << playerHP_max;
        cout << " | EP=" << fixed << setprecision(1) << playerEP_current << "/" << playerEP_max;
        cout << " | ATK=" << playerATK << " | END=" << playerEND << "\n";
        
        if(gateState == 1) cout << "[Gate of Opening ACTIVE]\n";
        if(gateState == 2) cout << "[Gate of Healing ACTIVE]\n";
        if(gateEndDebuff) cout << "[Gate End Debuff - Cannot reactivate]\n";
        
        cout << "\nARLONG: HP=" << enemyHP_current << "/" << enemyHP_max;
        cout << " | ATK=" << enemyATK << " | END=" << enemyEND;
        
        if(enemyPhase == 0) cout << " [BASE PHASE]\n";
        else if(enemyPhase == 1) cout << " [KIRIBACHI PHASE]\n";
        else if(enemyPhase == 2) cout << " [ENRAGED PHASE]\n";
        
        if(enemyIsStaggered) cout << "[STAGGERED]\n";
        if(enemyIsDisarmed) cout << "[DISARMED]\n";
        
        Border();
        
        // Begin Turn (Regen & Upkeep)
        BeginTurn();
        
        // Player Action
        bool turnEnded = false;
        while(!turnEnded) {
            PlayerActionMenu(turnEnded);
        }
        
        // Enemy Action
        if(!enemyIsStaggered) {
            EnemyAction();
        } else {
            cout << "\nArlong is STAGGERED and cannot attack this turn!\n";
            enemyIsStaggered = false;
        }
        
        // End of Turn
        TickCooldownsAndStatuses();
        UpdateEnemyPhase();
        enemyIsDisarmed = false;
        
        PressEnterToContinue();
        cout << "\n";
    }
    
    // Determine outcome
    if(playerHP_current > 0) {
        ShowEnding(true);
    } else {
        ShowEnding(false);
    }
}

void BeginTurn() {
    // EP Regeneration
    playerEP_current += playerEP_regen;
    if(playerEP_current > playerEP_max) {
        playerEP_current = playerEP_max;
    }
    
    cout << "\n[EP regenerated: +" << playerEP_regen << "]\n";
    
    // Gate Upkeep
    if(gateState == 1) {
        int cost = hasGateMastery ? G1_TURN_COST_MASTERY : G1_TURN_COST;
        playerEP_current -= cost;
        cout << "[Gate of Opening upkeep: -" << cost << " EP]\n";
        
        if(playerEP_current < 0) {
            cout << "\n[NOT ENOUGH EP! GATE CLOSING!]\n";
            EndGate();
        }
    } else if(gateState == 2) {
        int cost = G2_TURN_COST;
        playerEP_current -= cost;
        cout << "[Gate of Healing upkeep: -" << cost << " EP]\n";
        
        if(playerEP_current < 0) {
            cout << "\n[NOT ENOUGH EP! GATE CLOSING!]\n";
            EndGate();
        }
    }
    
    // Check if Gate End Debuff can be removed
    if(gateEndDebuff) {
        int requiredEP = ceil(1.5 * (hasGateMastery ? G1_CAST_COST_MASTERY : G1_CAST_COST));
        if(playerEP_current >= requiredEP) {
            gateEndDebuff = false;
            cout << "[Gate End Debuff removed - Gates can be activated again]\n";
        }
    }
    
    cout << "\n";
}

void PlayerActionMenu(bool &turnEnded) {
    cout << "Choose your action:\n";
    cout << "1. Basic Attack\n";
    cout << "2. Use Skill\n";
    cout << "3. Toggle Gate\n";
    cout << "Choice: ";
    
    int choice;
    cin >> choice;
    
    if(choice == 1) {
        // Basic Attack
        int damage = ceil(playerATK - enemyEND / 2.0);
        if(damage < 1) damage = 1;
        
        enemyHP_current -= damage;
        if(enemyHP_current < 0) enemyHP_current = 0;
        
        cout << "\nKeigan strikes Arlong for " << damage << " damage!\n";
        turnEnded = true;
        
    } else if(choice == 2) {
        // Use Skill
        if(skillCount == 0) {
            cout << "\nNo skills available!\n";
            return;
        }
        
        cout << "\nAvailable Skills:\n";
        for(int i = 0; i < skillCount; i++) {
            cout << (i+1) << ". " << name[i];
            cout << " (EP: " << epCost[i] << ")";
            if(cooldownLeft[i] > 0) {
                cout << " [CD: " << cooldownLeft[i] << "]";
            }
            cout << "\n";
        }
        cout << "0. Cancel\n";
        cout << "Select skill: ";
        
        int skillChoice;
        cin >> skillChoice;
        
        if(skillChoice == 0) {
            return;
        }
        
        if(skillChoice < 1 || skillChoice > skillCount) {
            cout << "\nInvalid skill selection.\n";
            return;
        }
        
        UseSkill(skillChoice - 1, turnEnded);
        
    } else if(choice == 3) {
        // Toggle Gate
        if(gateEndDebuff) {
            cout << "\nCannot activate Gate - Gate End Debuff is active!\n";
            cout << "Recover more EP to remove the debuff.\n";
            return;
        }
        
        if(gateState == 0) {
            // Activate G1
            int cost = hasGateMastery ? G1_CAST_COST_MASTERY : G1_CAST_COST;
            if(playerEP_current >= cost) {
                playerEP_current -= cost;
                gateState = 1;
                cout << "\n[GATE OF OPENING ACTIVATED!]\n";
                cout << "[ATK multiplied by 1.8!]\n";
                turnEnded = true;
            } else {
                cout << "\nNot enough EP! Need " << cost << " EP.\n";
            }
        } else if(gateState == 1 && hasGateMastery) {
            // Upgrade to G2
            if(playerEP_current >= G2_SWITCH_COST) {
                playerEP_current -= G2_SWITCH_COST;
                gateState = 2;
                cout << "\n[GATE OF HEALING ACTIVATED!]\n";
                cout << "[ATK multiplied by 2.2!]\n";
                turnEnded = true;
            } else {
                cout << "\nNot enough EP! Need " << G2_SWITCH_COST << " EP to switch.\n";
            }
        } else if(gateState >= 1) {
            // Close Gate
            cout << "\n[CLOSING GATE...]\n";
            EndGate();
            turnEnded = true;
        }
        
    } else {
        cout << "\nInvalid choice.\n";
    }
}

void UseSkill(int skillIndex, bool &turnEnded) {
     // Check cooldown
    if(cooldownLeft[skillIndex] > 0) {
        cout << "\nSkill is on cooldown! Wait " << cooldownLeft[skillIndex] << " more turns.\n";
        return;
    }
    
    // Check EP
    if(playerEP_current < epCost[skillIndex]) {
        cout << "\nNot enough EP! Need " << epCost[skillIndex] << " EP.\n";
        return;
    }
    
    // Check Gate requirement
    if(requiresGate[skillIndex] && gateState == 0) {
        cout << "\nThis skill requires a Gate to be active!\n";
        return;
    }
    
    // Check G2 requirement
    if(g2Only[skillIndex] && gateState != 2) {
        cout << "\nThis skill requires Gate of Healing (G2) to be active!\n";
        return;
    }
    
    // Check Chain requirement
    if(requiresChains[skillIndex] && !hasChainHandling) {
        cout << "\nThis skill requires Chain Handling!\n";
        return;
    }
    
    // Check Front Lotus requirement
    if(srequiresFrontLotusUsed[skillIndex] && !hasFrontLotusUsed) {
        cout << "\nThis skill requires Front Lotus to be used first!\n";
        return;
    }
    
    // Special handling for Gates
    if(id[skillIndex] == 1) {
        // Gate of Opening
        int cost = hasGateMastery ? G1_CAST_COST_MASTERY : G1_CAST_COST;
        if(playerEP_current >= cost) {
            playerEP_current -= cost;
            gateState = 1;
            cout << "\n[GATE OF OPENING ACTIVATED!]\n";
            cout << "[ATK multiplied by 1.8!]\n";
            turnEnded = true;
        } else {
            cout << "\nNot enough EP!\n";
        }
        return;
    }
    
    if(id[skillIndex] == 6) {
        // Gate of Healing
        if(gateState == 1) {
            if(playerEP_current >= G2_SWITCH_COST) {
                playerEP_current -= G2_SWITCH_COST;
                gateState = 2;
                cout << "\n[GATE OF HEALING ACTIVATED!]\n";
                cout << "[ATK multiplied by 2.2!]\n";
                turnEnded = true;
            } else {
                cout << "\nNot enough EP to switch gates!\n";
            }
        } else if(gateState == 0) {
            if(playerEP_current >= G2_CAST_COST) {
                playerEP_current -= G2_CAST_COST;
                gateState = 2;
                cout << "\n[GATE OF HEALING ACTIVATED!]\n";
                cout << "[ATK multiplied by 2.2!]\n";
                turnEnded = true;
            } else {
                cout << "\nNot enough EP!\n";
            }
        }
        return;
    }
    
    // Passive skill (Chain Handling)
    if(id[skillIndex] == 2) {
        cout << "\nChain Handling is a passive skill.\n";
        return;
    }
    
    // Execute attack skill
    playerEP_current -= epCost[skillIndex];
    cout << "\n" << playerName << " uses " << name[skillIndex] << "!\n";
    cout << "[-" << epCost[skillIndex] << " EP]\n\n";
    
    int totalDamage = 0;
    int effectiveHits = hits[skillIndex];
    
    // Add bonus hit for Reverse Lotus with G2
    if(id[skillIndex] == 8 && gateState == 2) {
        effectiveHits++;
        cout << "[G2 Bonus: +1 hit!]\n";
    }
    
    // Calculate damage per hit
    for(int i = 0; i < effectiveHits; i++) {
        int hitDamage;
        ApplyDamage(atkMultiplier[skillIndex], endPierceFactor[skillIndex], hitDamage);
        totalDamage += hitDamage;
        
        cout << "Hit " << (i+1) << ": " << hitDamage << " damage!\n";
    }
    
    cout << "\nTotal damage: " << totalDamage << "\n";
    
    // Apply special effects
    if(appliedStagger[skillIndex]) {
        enemyIsStaggered = true;
        cout << "[Arlong is STAGGERED!]\n";
    }
    
    if(disarmsKiribachi[skillIndex] && enemyPhase == 1) {
        enemyIsDisarmed = true;
        cout << "[Arlong's Kiribachi is DISARMED!]\n";
    }
    
    // Mark Front Lotus as used
    if(id[skillIndex] == 4) {
        hasFrontLotusUsed = true;
    }
    
    // Set cooldown
    if(cooldownMax[skillIndex] > 0) {
        cooldownLeft[skillIndex] = cooldownMax[skillIndex];
    }
    
    turnEnded = true;
}

void EnemyAction() {
    cout << "\n--- ARLONG'S TURN ---\n";
    
    int effectiveATK = enemyATK;
    
    // Reduce ATK if disarmed
    if(enemyIsDisarmed) {
        effectiveATK = effectiveATK * 3 / 4;
        cout << "Arlong fights without his weapon!\n";
    }
    
    int damage = ceil(effectiveATK - playerEND / 2.0);
    if(damage < 1) damage = 1;
    
    playerHP_current -= damage;
    if(playerHP_current < 0) playerHP_current = 0;
    
    if(enemyPhase == 0) {
        cout << "Arlong punches with brutal force!\n";
    } else if(enemyPhase == 1) {
        if(enemyIsDisarmed) {
            cout << "Arlong attacks with his bare fists!\n";
        } else {
            cout << "Arlong swings his massive Kiribachi!\n";
        }
    } else if(enemyPhase == 2) {
        cout << "Arlong attacks with primal rage!\n";
    }
    
    cout << "Keigan takes " << damage << " damage!\n";
}

void EndGate() {
    gateState = 0;
    
    // Apply Crash Penalty
    double crashDivisor = hasGateMastery ? 1.4 : 1.5;
    int originalATK = playerATK;
    int originalEND = playerEND;
    
    playerATK = ceil(playerATK / crashDivisor);
    playerEND = ceil(playerEND / crashDivisor);
    
    cout << "[GATE CRASH PENALTY!]\n";
    cout << "[ATK: " << originalATK << " -> " << playerATK << "]\n";
    cout << "[END: " << originalEND << " -> " << playerEND << "]\n";
    cout << "[Effect lasts 1 turn]\n";
    
    // Apply Gate End Debuff
    gateEndDebuff = true;
    cout << "[Gate End Debuff applied - Cannot reactivate gates until EP recovers]\n";
    
    // Add status to restore stats next turn
    if(statusCount < 5) {
        NAM[statusCount] = "Gate Crash";
        statusTurnsLeft[statusCount] = 1;
        statusATKDivisor[statusCount] = crashDivisor;
        statusENDDivisor[statusCount] = crashDivisor;
        statusPreventAction[statusCount] = false;
        statusCount++;
    }
}

void ApplyDamage(double atkMult, double endPierce, int &damageDealt) {
    double effectiveATK = playerATK;
    
    // Apply Gate multiplier
    if(gateState == 1) {
        effectiveATK *= 1.8;
    } else if(gateState == 2) {
        effectiveATK *= 2.2;
    }
    
    // Apply skill multiplier
    effectiveATK *= atkMult;
    
    // Calculate effective END after pierce
    double effectiveEND = enemyEND * (1.0 - endPierce);
    
    // Damage formula
    damageDealt = ceil(effectiveATK - effectiveEND / 2.0);
    if(damageDealt < 1) damageDealt = 1;
    
    enemyHP_current -= damageDealt;
    if(enemyHP_current < 0) enemyHP_current = 0;
}

void TickCooldownsAndStatuses() {
    // Tick cooldowns
    for(int i = 0; i < skillCount; i++) {
        if(cooldownLeft[i] > 0) {
            cooldownLeft[i]--;
        }
    }
    
    // Tick statuses and restore stats
    for(int i = 0; i < statusCount; i++) {
        if(statusTurnsLeft[i] > 0) {
            statusTurnsLeft[i]--;
            
            // If Gate Crash status expires, restore stats
            if(statusTurnsLeft[i] == 0 && NAM[i] == "Gate Crash") {
                // Restore ATK and END
                playerATK = ceil((double)playerATK * statusATKDivisor[i]);
                playerEND = ceil((double)playerEND * statusENDDivisor[i]);
                cout << "\n[Gate Crash Penalty expired - Stats restored]\n";
            }
        }
    }
    
    // Remove expired statuses
    int newCount = 0;
    for(int i = 0; i < statusCount; i++) {
        if(statusTurnsLeft[i] > 0) {
            NAM[newCount] = NAM[i];
            statusTurnsLeft[newCount] = statusTurnsLeft[i];
            statusATKDivisor[newCount] = statusATKDivisor[i];
            statusENDDivisor[newCount] = statusENDDivisor[i];
            statusPreventAction[newCount] = statusPreventAction[i];
            newCount++;
        }
    }
    statusCount = newCount;
}

void UpdateEnemyPhase() {
    int oldPhase = enemyPhase;
    
    if(enemyHP_current <= ARLONG_PHASE3_THRESHOLD && enemyPhase < 2) {
        enemyPhase = 2;
        enemyATK = 600;
        enemyEND = 660;
        
        Border();
        cout << "\n=== PHASE TRANSITION ===\n\n";
        cout << "Arlong roars in fury!\n\n";
        cout << "Arlong: \"ENOUGH! I'll tear you apart with my BARE HANDS!\"\n\n";
        cout << "[ARLONG ENTERS ENRAGED PHASE!]\n";
        cout << "[ATK increased to 600!]\n";
        cout << "[END increased to 660!]\n\n";
        PressEnterToContinue();
        
    } else if(enemyHP_current <= ARLONG_PHASE2_THRESHOLD && enemyPhase < 1) {
        enemyPhase = 1;
        enemyATK = 550;
        enemyEND = 600;
        
        Border();
        cout << "\n=== PHASE TRANSITION ===\n\n";
        cout << "Arlong grins menacingly and reaches for his weapon.\n\n";
        cout << "Arlong: \"You've forced me to use this. Behold my KIRIBACHI!\"\n\n";
        cout << "[ARLONG DRAWS HIS KIRIBACHI!]\n";
        cout << "[ATK increased to 550!]\n\n";
        PressEnterToContinue();
    }
}

void ShowEnding(bool win) {
    if(win) {
        Border();
        cout << "\n=== VICTORY ===\n\n";
        cout << "Arlong crashes to the ground, defeated.\n\n";
        cout << "Arlong (weakly): \"Impossible... a human... defeated me...?\"\n\n";
        cout << "Keigan stands over him, breathing heavily.\n\n";
        cout << "Keigan: \"This is for Bell-mere. For Nami. For Nojiko.\"\n";
        cout << "Keigan: \"For everyone you hurt.\"\n\n";
        cout << "Luffy (grinning): \"Nice job, Keigan!\"\n\n";
        cout << "The tower of Arlong Park begins to crumble.\n";
        cout << "Nami watches from below, tears streaming down her face.\n\n";
        cout << "Nami (crying): \"We're... we're finally free...\"\n\n";
        PressEnterToContinue();
        
        // Act 4: Freedom
        Border();
        cout << "\n=== ACT 4: COCOYASHI'S FREEDOM ===\n\n";
        cout << "The villagers of Cocoyashi pour out from their homes.\n";
        cout << "Laughter and tears mix as they celebrate their liberation.\n\n";
        cout << "Keigan collapses, his body drained from the battle.\n";
        cout << "Nami rushes to his side.\n\n";
        cout << "Nami: \"Keigan! Don't you dare die on us now!\"\n";
        cout << "Keigan (smiling weakly): \"I'm not... going anywhere...\"\n\n";
        cout << "Luffy pulls him up with a grin.\n";
        cout << "Luffy: \"Come on! We've got a three-day party to attend!\"\n\n";
        PressEnterToContinue();
        
        // Reunion
        Border();
        cout << "\n=== THE NEXT MORNING ===\n\n";
        cout << "At Bell-mere's grave, Nami and Keigan stand side by side.\n";
        cout << "Nojiko and Genzo watch from nearby, smiling.\n\n";
        cout << "Nami places tangerine blossoms on the grave.\n";
        cout << "Keigan bows deeply.\n\n";
        cout << "Keigan: \"I kept my promise, Bell-mere. The village is free.\"\n\n";
        cout << "Nami (softly): \"Thank you, Keigan. For everything.\"\n\n";
        cout << "Nojiko: \"You two... always so serious. Come on, smile!\"\n\n";
        PressEnterToContinue();
        
        // Recruitment
        Border();
        cout << "\n=== THREE DAYS LATER ===\n\n";
        cout << "At the docks, the Going Merry prepares to set sail.\n\n";
        cout << "Luffy turns to Keigan with his trademark grin.\n\n";
        cout << "Luffy: \"Oi, Keigan! You're strong - come with us!\"\n\n";
        cout << "Keigan freezes, caught off guard.\n";
        cout << "The crew watches expectantly.\n\n";
        cout << "Keigan (after a pause, smiling): \"Guess I'll handle the bookkeeping, then.\"\n\n";
        cout << "The Straw Hats erupt in cheers!\n";
        cout << "Nami smirks, already imagining her new navigator-partner.\n\n";
        cout << "Together, they board the ship.\n";
        cout << "Keigan glances back at the village one last time.\n\n";
        cout << "The sea breeze feels lighter than ever.\n\n";
        PressEnterToContinue();
        
        // Bounty Reveal
        Border();
        cout << "\n=== DAYS LATER - ABOARD THE GOING MERRY ===\n\n";
        cout << "Nami lounges on deck, reading the newspaper.\n";
        cout << "Two wanted posters flutter loose.\n\n";
        cout << "Keigan catches them before they hit the floor.\n";
        cout << "His eyes widen.\n\n";
        cout << "WANTED POSTERS:\n";
        cout << "- Straw Hat Monkey D. Luffy: 30,000,000 Beli\n";
        cout << "- Jet Lee Keigan: 5,000,000 Beli\n\n";
        cout << "The crew bursts into laughter.\n\n";
        cout << "Usopp: \"Jet? What, you planning to fly next time?\"\n\n";
        cout << "Keigan smirks, then laughs.\n";
        cout << "For the first time, his name carries weight across the seas.\n\n";
        cout << "The adventure continues...\n\n";
        cout << "=== THE END ===\n\n";
        cout << "Thank you for playing!\n\n";
        
    } else {
        // Alternate Ending - The Loop
        Border();
        cout << "\n=== DEFEAT ===\n\n";
        cout << "Keigan falls to his knees, vision blurring.\n\n";
        cout << "Arlong (laughing): \"Still weak! Still human!\"\n\n";
        cout << "Arlong grabs Keigan and hurls him into the sea once more.\n\n";
        cout << "Keigan: \"No... not again...\"\n\n";
        cout << "The water engulfs him. Darkness takes over.\n\n";
        PressEnterToContinue();
        
        Border();
        cout << "\n=== ??? ===\n\n";
        cout << "...\n\n";
        cout << "......\n\n";
        cout << ".........\n\n";
        PressEnterToContinue();
        
        Border();
        cout << "\nWhen Keigan's eyes open, he's standing in front of Bell-mere's house.\n\n";
        cout << "The village is whole. Arlong never came.\n";
        cout << "Yet a heavy dread lingers in his heart.\n\n";
        cout << "Keigan (to himself): \"Was it... all a dream?\"\n";
        cout << "Keigan: \"Can I really defeat Arlong?\"\n\n";
        cout << "The vision dissolves into silence.\n\n";
        cout << "=== TRY AGAIN ===\n\n";
        cout << "Your training was insufficient.\n";
        cout << "Consider focusing more on combat stats and unlocking key skills.\n\n";
        cout << "GAME OVER\n\n";
    }
}
