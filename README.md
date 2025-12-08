# Keigan's Journey: Cocoyashi Arc

- A text-based RPG inspired by One Piece, featuring strategic turn-based combat and a character progression system based on the Eight Inner Gates technique.
- Developed by: Von Justin L. Estayani | ZC11
## Table of Contents
- [Overview](#overview)
- [How to Run](#how-to-run)
- [Gameplay Structure](#gameplay-structure)
- [Controls](#controls)
- [System Requirements](#system-requirements)

---

## Overview

**Keigan's Journey: Cocoyashi Arc** is a narrative-driven turn-based RPG where you play as Keigan "Kei" Lee, a survivor seeking revenge against the tyrannical Fishman pirate Arlong. The game features:

- **Story-driven gameplay** with 4 acts
- **48-turn training arc** where you customize your character
- **Strategic combat system** with Gates, Skills, and EP management
- **Multiple skill paths** unlockable through Self-Discovery
- **Dynamic boss battle** with three distinct phases

---

## How to Run

### Prerequisites
- C++ compiler (supporting C++11 or later)
  - **Windows**: MinGW, MSVC, or similar
  - **macOS/Linux**: g++ or clang++

### Compilation

#### Using g++ (Linux/macOS/MinGW on Windows)
```bash
g++ -std=c++11 keigans_journey.cpp -o keigans_journey
```

#### Using MSVC (Windows)
```bash
cl /EHsc keigans_journey.cpp
```

### Running the Game

#### Linux/macOS
```bash
./keigans_journey
```

#### Windows
```bash
keigans_journey.exe
```

---

## Gameplay Structure

### Act 1: The Fall of Cocoyashi
- Narrative introduction
- Witness Bell-mere's death and Arlong's cruelty
- Keigan is thrown into the sea

### Act 2: The Eight-Year Training Arc (48 Turns)
Each turn represents 2 months of training. You have **48 total turns** to prepare for the final battle.

**Training Options:**
1. **HP Training** - Increases max HP (Formula: 21 + 4n per session)
2. **ATK Training** - Increases attack power (Formula: 11 + 2n per session)
3. **END Training** - Increases endurance (Formula: 15 + 3n per session)
4. **EP Training** - Increases energy pool and regeneration (Formula: 15 + 3n pool, 2.5 + 0.5n regen)
5. **Self-Discovery** - Unlocks new combat skills (8 available)

**Self-Discovery Progression:**
1. Gate of Opening (G1)
2. Chain Handling (Passive)
3. Kei-ga-n Barrage
4. Front Lotus (Omote Renge)
5. Gate Mastery (Passive)
6. Gate of Healing (G2)
7. Chain Barrage
8. Reverse Lotus (Ura Renge)

### Act 3: Battle at Arlong Park
- Strategic turn-based combat against Arlong
- Three enemy phases with increasing difficulty
- Victory or defeat ending based on performance

### Act 4: Cocoyashi's Freedom (Victory Only)
- Celebration and character resolution
- Recruitment into the Straw Hat Pirates
- Bounty reveal

---

## Controls

### Main Menu
- `1` - Start Game
- `2` - Quit

### Training Phase
- `1` - HP Training
- `2` - ATK Training
- `3` - END Training
- `4` - EP Training
- `5` - Self-Discovery (if available)
- Preview stats before confirming each choice

### Battle Phase
**Action Menu:**
- `1` - Basic Attack
- `2` - Use Skill
- `3` - Toggle Gate

**Skill Selection:**
- Enter skill number (1-8 depending on unlocked skills)
- `0` - Cancel and return to action menu

---

## System Requirements

- **OS**: Windows, macOS, or Linux
- **Compiler**: C++11 or later
- **Memory**: Minimal (< 50 MB)
- **Display**: Console/Terminal capable of displaying 80 characters width

---

## Tips for New Players

1. **Balance your training** - Don't neglect any stat category
2. **Prioritize Self-Discovery** early to unlock powerful skills
3. **Gate Mastery is crucial** - Reduces Gate costs significantly
4. **EP management is key** - Without sufficient EP, Gates become unusable
5. **Front Lotus unlocks Reverse Lotus** - Plan your skill progression
6. **Arlong has 2500 HP** - Calculate if your damage output is sufficient

---

## License

Educational project - Free to use and modify.

---

**Enjoy your journey!**
