#include <iostream>
#include <string>
#include <cmath>
#include <iomanip>
#include <limits>
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
string statusName[5];
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

// Training history [5]: HP, ATK, END, EP, Self-Discovery
int trainingHistory[5] = {0, 0, 0, 0, 0};

// FUNCTIONS DECLARATION
int GetValidChoice();
void InitGame();
void Border();
void PressEnterToContinue();
void Loading();
void DisplayMainMenu();
void DisplayAct1();
void DisplayAct2Intro();
void StartBattle();
void DisplayTrainingStats(int turnsLeft);
void PreviewStatGain(int statType);
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

    // Ask the user to play again and restart the whole game
    cout << "\n\nDo you want to play again? (1 = Yes, 0 = No): ";
    int playAgain = GetValidChoice();
    
    if (playAgain == 1) {
        main();
    } else {
        cout << "\nThank you for playing!\n";
        return 0;
    }
}

// FUNCTIONS INITIALIZATION

int GetValidChoice() {  
    int value;
    while (true) {
        cin >> value;

        if (!cin.fail()) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }

        // if input is not an integer
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
}

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
}

void Border() {
    cout << "====================================================================";
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

    // After finishing, clear the whole line (Loading...)
    cout << '\r' << string(20, ' ') << '\r';
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
    cout << "\nMain Menu";
    cout << "\n1. Start\n";
    cout << "2. Quit\n";
    cout << "\nEnter your choice (1 or 2): ";

    int choice = GetValidChoice();
    
    if (choice == 2) {
        cout << "\nThank you for playing!\n";
        exit(0); // exit the game immediately
    }

    while(choice < 1 || choice > 2) {
            cout << "Invalid choice. Please select 1-2: ";
            choice = GetValidChoice();

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

    cout << "\nCocoyashi Village, once peaceful, falls into darkness as Arlong and his monstrous crew arrive.\n";
    cout << "The villagers tremble as the Fishman captain declares his rule.\n\n";
    cout << "Arlong: \"From this day on, your lives belong to me. Pay tribute... or die!\"\n\n";
    cout << "Bell-mere, defiant yet afraid, steps forward. She has money only for Nami and Nojiko.\n";
    cout << "Arlong's grin widens as he realizes the truth.\n\n";
    cout << "Arlong: \"So you choose to die? Very well...\"\n\n";
    cout << "A gunshot rings through the village. Nami and Nojiko scream as Bell-mere falls.\n";
    cout << "Keigan watches, frozen in shock, as his world shatters before him.\n\n";
    cout << "Keigan: \"ARLOOOOONG!!!\"\n\n";
    cout << "Overcome with rage, young Keigan charges at the towering Fishman.\n";
    cout << "His fist lands squarely on Arlong's jaw... but it has no effect.\n\n";
    cout << "Arlong (laughing): \"Hahaha! Is that all? Weak humans like you are nothing to me!\"\n";
    cout << "Arlong grabs Keigan by the throat, lifting him effortlessly.\n\n";
    cout << "Nami: \"Keigan! Stop! Run!\"\n";
    cout << "Nojiko: \"Please don't hurt him!\"\n\n";
    cout << "Arlong: \"Let this be a lesson. Weak humans... belong to the deep!\"\n\n";
    cout << "With brutal force, Arlong hurls Keigan into the open sea, leaving him to drown.\n";
    cout << "The villagers watch helplessly as the waves swallow him.\n\n";
    cout << "But fate has other plans...\n";
    PressEnterToContinue();
    Border();
    cout << "\nDays later, Keigan washes ashore on a distant island.\n";
    cout << "Bruised, exhausted, barely clinging to life, he opens his eyes to an empty sky.\n";
    cout << "Memories of Bell-mere's final smile haunt him, fueling the fire in his heart.\n\n";
    cout << "Keigan (weakly): \"I couldn't save you... but I will not stay weak.\"\n";
    cout << "He forces himself to stand, trembling but determined.\n\n";
    cout << "Keigan (whispers): \"I will return... I'll train, I'll grow stronger...\"\n";
    cout << "Keigan (furious whisper): \"And I will destroy you, Arlong.\"\n\n";
    PressEnterToContinue();
}

void DisplayAct2Intro() {
    Border();
    cout << "\nACT 2: THE EIGHT-YEAR TRAINING ARC\n";
    Border();

    cout << "\nDrifting unconscious for days, Keigan is finally rescued by the Good Heart Pirates.\n";
    cout << "Their doctor works tirelessly to keep him alive, refusing to let the sea claim him.\n\n";
    cout << "Doctor: \"This boy... he's barely breathing, but he's fighting. He wants to live.\"\n";
    cout << "Captain: \"Then let's give him the chance the world didn't.\"\n\n";
    cout << "When Keigan awakens days later, the Captain sits beside him.\n";
    cout << "Keigan: \"Why... why did you save me?\"\n";
    cout << "Captain: \"Because you're still alive. And the sea only spares those with a purpose.\"\n";
    cout << "Captain: \"Live first. Then choose what to do with the life you still have.\"\n\n";
    cout << "They leave him on a quiet, uninhabited island-safe, silent, and full of possibility.\n";
    cout << "Before departing, the Captain sets down a small satchel of supplies.\n";
    cout << "Captain: \"Survive. Grow. And when you're ready... choose your path.\"\n\n";
    cout << "Days later, while exploring, Keigan stumbles into a moss-covered cave.\n";
    cout << "There he finds an ancient, dust-covered scroll resting on a stone pedestal.\n\n";
    cout << "Keigan: \"What's this...?\"\n";
    cout << "The scroll reads: \"The Eight Inner Gates\" - a forbidden training art.\n\n";
    cout << "Keigan studies the scroll carefully. It speaks of unlocking hidden strength,\n";
    cout << "but at great risk to one's own body.\n\n";
    cout << "Keigan: \"If this is what it takes... then I'll do it. I have to.\"\n\n";
    cout << "From that day forward, training becomes his entire existence.\n";
    cout << "Mornings spent striking trees until his knuckles bleed.\n";
    cout << "Afternoons climbing cliffs and battling the island's currents.\n";
    cout << "Nights meditating under waterfalls, learning to control his energy.\n\n";
    cout << "Each dawn brings pain. Each sunset brings progress.\n";
    cout << "Days turn into months. Months into years.\n\n";
    cout << "Keigan: \"I won't stay weak... not anymore.\"\n\n";
    cout << "Eight years of relentless training await...\n";
    PressEnterToContinue();
}

void StartBattle() {
    Border();
    cout << "\nACT 3: RETURN TO COCOYASHI\n";
    Border();

    cout << "\nEight years have passed. Keigan's body has hardened, his mind sharpened, his purpose unshaken.\n";
    cout << "With Bell-mere's old headband tied to his belt, he sets sail once more-toward the life he left behind.\n\n";
    cout << "His journey leads him to Baratie, a floating restaurant full of noise, food, and laughter.\n";
    cout << "There, he sees a strange but lively crew-the Straw Hat Pirates.\n";
    cout << "Among them, a sharp-eyed navigator moves with caution. Something about her stirs a memory.\n\n";
    cout << "Keigan (quietly): \"Why does she feel familiar...?\"\n\n";
    cout << "But before he can dwell on it, chaos erupts.\n";
    cout << "Don Krieg attacks with a full armada, and Dracule Mihawk arrives-cutting ships like paper.\n";
    cout << "Zoro challenges Mihawk and falls, bloodied but unwilling to break.\n\n";
    cout << "In the confusion, the navigator and a few Straw Hats flee aboard the Going Merry.\n";
    cout << "Keigan hears her name spoken clearly for the first time.\n\n";
    cout << "Keigan (stunned, whispering): \"Nami...?\"\n\n";
    cout << "The name hits him like a wave. His chest tightens with a memory of sunlit days and a promise he failed to keep.\n";
    cout << "When Zoro, Usopp, and the others chase after her, Keigan joins them, guided by an instinct eight years buried.\n\n";
    cout << "The chase leads them straight to the Conomi Islands... to Cocoyashi Village.\n\n";
    PressEnterToContinue();
    Border();

    cout << "\nThe village they find is silent and fearful.\n";
    cout << "Homes are worn. Faces are tired. Children look over their shoulders as if expecting punishment.\n\n";
    cout << "Nojiko steps forward, guarded but calm.\n";
    cout << "With Luffy and Sanji now reunited with the group, she tells them everything:\n";
    cout << "Bell-mere's death... the tribute... Arlong's rule... and Nami's forced servitude.\n\n";
    cout << "When Nojiko mentions a boy from long ago, the group turns slowly toward Keigan.\n\n";
    cout << "Nojiko: \"There was a boy once... Keigan. Arlong threw him into the sea and left him for dead.\"\n";
    cout << "Usopp: \"Wait-YOU? That was you?!\"\n";
    cout << "Zoro: \"Tch. Surviving that... you're tougher than you look.\"\n";
    cout << "Sanji: \"Guess fate dragged you back here, curly-eyes.\"\n";
    cout << "Luffy (smiling widely): \"So you're here to help us fight!\"\n\n";
    cout << "Keigan (softly, steady): \"I swore I'd return. I'm here to end what started eight years ago.\"\n\n";
    cout << "The Straw Hats accept him without hesitation. Their shared purpose becomes clear:\n";
    cout << "Free Cocoyashi. Save Nami. End Arlong's rule.\n\n";
    PressEnterToContinue();
    Border();

    cout << "\nThe march to Arlong Park begins.\n";
    cout << "Marine cowards try to interfere, but Luffy sends them flying without breaking stride.\n";
    cout << "Fishman sentries tower over them, but Zoro cuts through their ranks effortlessly.\n";
    cout << "Usopp shouts bravely. Sanji strikes cleanly. Every step shakes the ground with resolve.\n\n";
    cout << "The gates of Arlong Park rise ahead-massive, sun-bleached, and oppressive.\n";
    cout << "They look exactly as Keigan remembers... and nothing like the nightmare he survived.\n\n";
    cout << "Keigan (under his breath): \"This time... I bring what I lacked eight years ago. Strength. Purpose. Resolve.\"\n\n";
    cout << "They break through the gates.\n\n";
    cout << "Arlong stands at the center, teeth bared in a monstrous grin.\n";
    cout << "He turns his gaze toward the group... then freezes when he sees Keigan.\n\n";
    cout << "Keigan steps forward, the air around him heavy and focused.\n\n";
    cout << "Keigan: \"I'm not the boy you threw away. I'm the one who's come to end your rule.\"\n\n";
    cout << "Luffy: \"Keigan! Time to show him what eight years of training can do!\"\n";
    cout << "Zoro: \"Don't hold back.\"\n";
    cout << "Sanji: \"Kick his teeth in for Nami.\"\n\n";
    cout << "Keigan clenches his fists. The ground trembles.\n\n";
    cout << "THE BATTLE BEGINS!\n";
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
    cout << "  EP Regen: " << fixed << setprecision(1) << playerEP_regen << "\n\n";
    
    cout << "Training History:\n";
    cout << "  HP Sessions: " << trainingHistory[0] << "\n";
    cout << "  ATK Sessions: " << trainingHistory[1] << "\n";
    cout << "  END Sessions: " << trainingHistory[2] << "\n";
    cout << "  EP Sessions: " << trainingHistory[3] << "\n";
    cout << "  Self-Discovery: " << trainingHistory[4] << "\n";
    Border();
}

void PreviewStatGain(int statType) {
    int n = trainingHistory[statType - 1];
    
    switch(statType) {
        case 1: // HP
            cout << "\n[Preview] HP will increase by " << 21 + 4 * n << ".\n";
            cout << "New HP: " << (playerHP_max + (21 + 4 * n)) << "\n";
            break;
        case 2: // ATK
            cout << "\n[Preview] ATK will increase by " << 11 + 2 * n << ".\n";
            cout << "New ATK: " << (playerATK + (11 + 2 * n)) << "\n";
            break;
        case 3: // END
            cout << "\n[Preview] END will increase by " << 15 + 3 * n << ".\n";
            cout << "New END: " << (playerEND + (15 + 3 * n)) << "\n";
            break;
        case 4: // EP
            cout << "\n[Preview] EP Pool will increase by " << 15 + 3 * n << ".\n";
            cout << "New EP Pool: " << (playerEP_max + (15 + 3 * n)) << "\n";
            cout << "\n[Preview] EP Regen will increase by " << 2.5 + 0.5 * n << ".\n";
            cout << "New EP Regen: " << (playerEP_regen + (2.5 + 0.5 * n)) << "\n";
            break;
        case 5: { // Self-Discovery
            int sdCount = trainingHistory[4] + 1;  // +1 because we are previewing the next skill to unlock

            cout << "\n[Preview] You will unlock: ";

            switch (sdCount) {
                case 1:  cout << "Gate of Opening (G1)"; break;
                case 2:  cout << "Chain Handling"; break;
                case 3:  cout << "Kei-ga-n Barrage"; break;
                case 4:  cout << "Front Lotus (Omote Renge)"; break;
                case 5:  cout << "Gate of Opening - Mastery"; break;
                case 6:  cout << "Gate of Healing (G2)"; break;
                case 7:  cout << "Chain Barrage"; break;
                case 8:  cout << "Reverse Lotus (Ura Renge)"; break;
            }
            cout << ".\n";
            break;
        }
    }
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
        cout << "4. EP Training\n";
        
        bool allowSD = (trainingHistory[4] < 8);

        if (allowSD) {
            cout << "5. Self-Discovery (Unlock Skills)\n";
        }

        int choice;
        while (true) {
            cout << "Enter your choice: ";

            choice = GetValidChoice();

            if ((choice >= 1 && choice <= 4) || (choice == 5 && allowSD)) {

                // PREVIEW SECTION
                PreviewStatGain(choice);

                cout << "\nConfirm this training?\n";
                cout << "1 = Yes\n2 = No, choose again\n";
                cout << "Enter your choice: ";

                int confirm = GetValidChoice();

                if (confirm == 1) break;   // confirmed
                else continue;             // go back to choose again
            }

            if (allowSD)
                cout << "Invalid choice. Please enter (1-6).\n";
            else
                cout << "Invalid choice. Please enter (1-5).\n";
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
    cout << "  EP Regen: " << fixed << setprecision(1) << playerEP_regen << "\n";
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
        case 4: { // EP
            double gainPool = 15 + 3 * n;
            double gainRegen = 2.5 + 0.5 * n;
            playerEP_max += gainPool;
            playerEP_regen += gainRegen;
            trainingHistory[3]++;
            cout << "\n[EP Pool increased by " << gainPool << "!]";
            cout << "\n[EP Regen increased by " << gainRegen << "!]";
            break;
        }

        case 5: { // Self-Discovery
            trainingHistory[4]++;
            int sdCount = trainingHistory[4];
            
            if(sdCount == 1) {
                AddSkill(1); // Gate of Opening (G1)
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
                AddSkill(5); // Gate of Opening - Mastery
                hasGateMastery = true;
                cout << "\n[Unlocked: Gate of Opening - Mastery!]";
            } else if(sdCount == 6) {
                AddSkill(6); // Gate of Healing (G2)
                cout << "\n[Unlocked: Gate of Healing (G2)!]";
            } else if(sdCount == 7) {
                AddSkill(7); // Chain Barrage
                cout << "\n[Unlocked: Chain Barrage!]";
            } else if(sdCount == 8) {
                AddSkill(8); // Reverse Lotus
                cout << "\n[Unlocked: Reverse Lotus (Ura Renge)!]";
            } else {
                cout << "\n[Self-Discovery session completed.]";
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
            
        case 2: // Chain Handling (Passive)
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
        
        case 5: // Gate of Opening - Mastery (Passive)
            name[index] = "Gate of Opening - Mastery";
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
        cout << "\n TURN " << turnNumber << " \n\n";
        
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
    cout << "Enter your choice (1-3): ";
    
    int choice = GetValidChoice();
    
    if(choice == 1) {
        // Basic Attack
        int damage = ceil((playerATK - enemyEND) / 2.0);
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
        
        int skillChoice = GetValidChoice();
        
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

    if(id[skillIndex] == 5) {
        cout << "\nGate of Opening - Mastery is a passive skill.\n";
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
    
    int damage = ceil((effectiveATK - playerEND) / 2.0);
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
    cout << "[ATK: " << originalATK << " -> " << playerATK << " (÷" << crashDivisor << ")]\n";
    cout << "[END: " << originalEND << " -> " << playerEND << " (÷" << crashDivisor << ")]\n";
    cout << "[Effect lasts 1 turn]\n";
    
    // Apply Gate End Debuff
    gateEndDebuff = true;
    int requiredEP = ceil(1.5 * (hasGateMastery ? G1_CAST_COST_MASTERY : G1_CAST_COST));
    cout << "[Gate End Debuff applied - Cannot reactivate until EP ≥ " << requiredEP << "]\n";
    
    // Add status to restore stats next turn
    if(statusCount < 5) {
        statusName[statusCount] = "Gate Crash";
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
    damageDealt = ceil((effectiveATK - effectiveEND) / 2.0);
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
            if(statusTurnsLeft[i] == 0 && statusName[i] == "Gate Crash") {
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
            statusName[newCount] = statusName[i];
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
        cout << "\n PHASE TRANSITION \n\n";
        cout << "Arlong roars in fury!\n\n";
        cout << "Arlong: \"ENOUGH! I'll tear you apart with my BARE HANDS!\"\n\n";
        cout << "[ARLONG ENTERS ENRAGED PHASE!]\n";
        cout << "[ATK increased to 600!]\n";
        cout << "[END increased to 660!]\n";
        Border();
        PressEnterToContinue();
        cout << "\n";
        
    } else if(enemyHP_current <= ARLONG_PHASE2_THRESHOLD && enemyPhase < 1) {
        enemyPhase = 1;
        enemyATK = 550;
        enemyEND = 600;
        
        Border();
        cout << "\n PHASE TRANSITION \n\n";
        cout << "Arlong grins menacingly and reaches for his weapon.\n\n";
        cout << "Arlong: \"You've forced me to use this. Behold my KIRIBACHI!\"\n\n";
        cout << "[ARLONG DRAWS HIS KIRIBACHI!]\n";
        cout << "[ATK increased to 550!]\n";
        Border();
        PressEnterToContinue();
    }
}

void ShowEnding(bool win) {
    if(win) {
        Border();
        cout << "\n VICTORY \n\n";
        cout << "Keigan's final strike lands clean.\n";
        cout << "Arlong's jaw snaps backward as the fishman crashes into the ruins of his tower.\n\n";
        
        cout << "Arlong (choking, furious): \"No... I am perfection... I am superior...!\"\n\n";
        
        cout << "Keigan stands over him—bloodied, shaking, but unbroken.\n\n";
        cout << "Keigan (voice trembling, rising in strength):\n";
        cout << "\"This is for Bell-mere... for Nami... for Nojiko... and for every life you took.\"\n\n";
        
        cout << "Arlong's eyes widen at the fury in Keigan's voice.\n";
        cout << "The fishman's body falls still.\n\n";
        
        cout << "Luffy lands beside Keigan, smiling despite his bruises.\n";
        cout << "Luffy: \"You did it. You really did it, Keigan!\"\n\n";
        
        cout << "Above them, Arlong Park begins to collapse.\n";
        cout << "Nami watches from below, tears streaming down her face.\n\n";
        cout << "Nami (crying): \"We're... finally free...\"\n\n";
        PressEnterToContinue();

        // Act 4 – Cocoyashi's Freedom
        Border();
        cout << "\n ACT 4: COCOYASHI'S FREEDOM \n\n";
        
        cout << "The villagers flood the streets.\n";
        cout << "Mothers clutch their children, elders cry openly, and the air fills with cheers.\n\n";
        
        cout << "Keigan collapses to one knee, his body trembling from the strain of the Gates.\n";
        cout << "Nami rushes to his side.\n\n";
        
        cout << "Nami (panicked): \"Keigan! Stay with me—don't close your eyes!\"\n";
        cout << "Keigan (weak smile): \"After all this? I'm not going anywhere...\"\n\n";
        
        cout << "Luffy lifts him with a grin.\n";
        cout << "Luffy: \"Oi! No passing out! We've got a THREE-DAY PARTY waiting!\"\n\n";
        
        PressEnterToContinue();

        // Reunion Scene
        Border();
        cout << "\n THE NEXT MORNING \n\n";
        cout << "Sunlight warms Bell-mere's grave.\n";
        cout << "Nami and Keigan stand quietly before it.\n\n";
        
        cout << "Nami places tangerine blossoms atop the soft earth.\n";
        cout << "Nojiko smiles gently from the side.\n\n";
        
        cout << "Keigan bows deeply.\n";
        cout << "Keigan: \"Bell-mere... I kept my promise. Your girls are free.\"\n\n";
        
        cout << "Nami's eyes shimmer.\n";
        cout << "Nami (softly): \"Thank you, Keigan. For fighting beside us... for believing in me.\"\n\n";
        
        cout << "Nojiko (teasing): \"You two… you're so dramatic in the mornings. Relax a little!\"\n\n";
        
        PressEnterToContinue();

        // Recruitment
        Border();
        cout << "\n THREE DAYS LATER \n\n";
        cout << "At the docks, the Going Merry rocks gently with the waves.\n";
        cout << "Villagers gather to send off their heroes.\n\n";
        
        cout << "Luffy stomps excitedly on the deck.\n";
        cout << "Luffy: \"OI, KEIGAN! You're strong—join my crew!\"\n\n";
        
        cout << "Keigan freezes. Nami looks at him, hope flickering in her eyes.\n";
        cout << "Zoro raises an eyebrow. Usopp leans forward.\n\n";
        
        cout << "After a long breath, Keigan smiles.\n";
        cout << "Keigan: \"Fine. I'll come. Somebody has to handle the bookkeeping.\"\n\n";
        
        cout << "The Straw Hats erupt into cheers.\n";
        cout << "Nami smirks, nudging Keigan's arm.\n";
        cout << "Nami: \"Welcome aboard, partner.\"\n\n";
        
        cout << "The ship sails forward.\n";
        cout << "Keigan looks back at his village one last time... then toward the open sea.\n\n";
        
        PressEnterToContinue();

        // Bounty Reveal
        Border();
        cout << "\n DAYS LATER - ABOARD THE GOING MERRY \n\n";
        
        cout << "Nami lounges on deck, flipping through a newspaper.\n";
        cout << "Two wanted posters flutter loose.\n\n";
        
        cout << "Keigan catches them.\n\n";
        
        cout << "WANTED POSTERS:\n";
        cout << "- Monkey D. Luffy: 30,000,000 Beli\n";
        cout << "- \"Jet\" Lee Keigan: 5,000,000 Beli\n\n";
        
        cout << "Usopp bursts out laughing.\n";
        cout << "Usopp: \"JET?! What are you, a bullet now?!\"\n\n";
        
        cout << "Keigan smirks, shaking his head.\n";
        cout << "Keigan: \"Guess people noticed me after all.\"\n\n";
        
        cout << "The waves crash softly.\n";
        cout << "The adventure continues...\n\n";
        
        cout << " THE END \n\n";
        cout << "Thank you for playing!\n\n";
        
    } else {
        // Alternate Ending – The Loop
        Border();
        cout << "\n DEFEAT \n\n";
        cout << "Keigan falls to his knees.\n";
        cout << "The Gates flicker out. His body gives way.\n\n";
        
        cout << "Arlong towers over him, shadow swallowing the light.\n\n";
        
        cout << "Arlong (laughing maniacally):\n";
        cout << "\"Still weak... still human! Know your place!\"\n\n";
        
        cout << "He grabs Keigan by the throat and hurls him into the ocean.\n\n";
        cout << "Keigan's vision darkens as he sinks.\n";
        cout << "His heartbeat slows...\n";
        cout << "Fades...\n\n";
        
        PressEnterToContinue();

        // The Loop Resets
        Border();
        cout << "\n A FAMILIAR MORNING \n\n";
        cout << "Keigan gasps awake.\n\n";
        
        cout << "He stands in front of Bell-mere's house.\n";
        cout << "The air is warm. Peaceful. Too peaceful.\n\n";
        
        cout << "Children run past. Villagers smile. No Arlong. No ruins.\n";
        cout << "Everything is perfect—too perfect.\n\n";
        
        cout << "Keigan (whispering): \"No... this isn't right... did I... fail?\"\n";
        cout << "A cold dread grips his heart as reality flickers.\n\n";
        
        cout << "A voice echoes behind him.\n";
        cout << "\"Train harder... or repeat this forever.\"\n\n";
        
        cout << "The world begins to dissolve.\n\n";
        
        cout << " TRY AGAIN \n\n";
        cout << "Your strength was not enough to break the cycle.\n";
        cout << "Return stronger. The village depends on you.\n\n";
        
        cout << "GAME OVER\n\n";
    }
}
