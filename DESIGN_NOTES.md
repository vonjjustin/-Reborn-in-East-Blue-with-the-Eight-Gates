# Design Notes: Keigan's Journey

## Core Design Philosophy

This game was designed as a **narrative-driven strategic RPG** that emphasizes **player choice**, **resource management**, and **long-term planning**. The design draws inspiration from anime combat systems while maintaining accessibility through text-based gameplay.

---

## Key Design Decisions

### 1. Training Arc Structure (48 Turns)

**Rationale:**
- 48 turns represents 8 years (6 turns per year, 2 months per turn)
- Creates meaningful scarcity: players must choose wisely
- Forces strategic planning rather than "maxing everything"
- Scaling formulas reward specialization while preventing single-stat dominance

**Formula Design:**
- Base values ensure minimum viability
- Linear scaling with turn count (n) rewards repeated investment
- Self-Discovery limited to 8 unlocks creates hard trade-offs

### 2. Gate System (Energy Management)

**Inspiration:** Naruto's Eight Inner Gates technique

**Mechanics:**
- **Gate of Opening (G1)**: 1.8× ATK multiplier, moderate cost
- **Gate of Healing (G2)**: 2.2× ATK multiplier, higher cost
- **Mastery System**: Reduces costs, enables G1→G2 switching

**Crash Penalty Design:**
```
On Gate closure:
- Stats divided by 1.5 (or 1.4 with Mastery)
- Gate End Debuff applied
- Cannot reactivate until EP ≥ 1.5× cast cost
```

**Purpose:**
- Gates provide burst damage but aren't "always on"
- Crash penalty creates risk/reward tension
- EP management becomes critical strategic layer
- Prevents trivializing the boss fight

### 3. Skill Progression System

**Eight-Skill Tree:**
```
1. Gate of Opening → Foundation skill
2. Chain Handling → Unlocks chain-based skills
3. Kei-ga-n Barrage → Early multi-hit damage
4. Front Lotus → High damage, stagger effect, requires chains
5. Gate Mastery → Passive efficiency upgrade
6. Gate of Healing → G2 access
7. Chain Barrage → Advanced chain skill
8. Reverse Lotus → Ultimate skill, requires Front Lotus
```

**Design Principles:**
- **Linear progression**: Skills unlock in order (no branching)
- **Dependency chains**: Advanced skills require earlier unlocks
- **Passive milestones**: Chain Handling and Gate Mastery are force multipliers
- **Ultimate payoff**: Reverse Lotus is powerful but requires full investment

### 4. Combat System

**Core Formula:**
```
Damage = ⌈(Effective ATK - Effective END) / 2⌉
Minimum damage = 1
```

**Damage Calculation Layers:**
1. Base ATK × Gate Multiplier (1.8 or 2.2)
2. Result × Skill Multiplier (0.45 to 1.25)
3. END reduced by Pierce Factor (5% to 35%)
4. Apply damage formula
5. Multiply by hit count

**Why This Formula?**
- Simple and transparent
- Prevents negative damage
- END becomes meaningful defense
- Pierce mechanics reward skill usage
- Multi-hit skills create consistent DPS

### 5. Enemy Phase System

**Arlong's Three Phases:**

| Phase | HP Threshold | ATK | END | Behavior |
|-------|-------------|-----|-----|----------|
| Base | 2500-1601 | 520 | 600 | Basic attacks |
| Kiribachi | 1600-901 | 550 | 600 | Weapon attacks, can be disarmed |
| Enraged | 900-0 | 600 | 660 | Maximum aggression |

**Design Intent:**
- Escalating difficulty curve
- Visual/narrative feedback (weapon draw, rage)
- Disarm mechanic only relevant in Phase 2
- Forces adaptation mid-battle

### 6. Status Effects & Stagger

**Stagger Mechanic:**
- Applied by Front Lotus and Reverse Lotus
- Enemy skips next turn
- Lasts 1 turn only
- Creates breathing room for EP recovery

**Disarm Mechanic:**
- Front Lotus only
- Reduces enemy ATK by 25% (×0.75)
- Only works in Kiribachi phase
- Rewards skill timing and phase awareness

### 7. EP (Energy Point) Economy

**EP Sources:**
- Base pool: 5 (starting)
- Training: +15 to +45 per session (scaling)
- Regen: 2.5 to 7.5+ per turn (scaling)

**EP Costs:**
- G1 Cast: 35 (30 with Mastery)
- G1 Upkeep: 7/turn (5 with Mastery)
- G2 Cast: 45
- G2 Upkeep: 9/turn
- G2 Switch: 10 (from G1 to G2)
- Skills: 12-25 per use

**Balance Philosophy:**
- Early game: Can't sustain Gates long
- Mid game: Can activate but limited uptime
- Late game: Can sustain G1 indefinitely, G2 with planning
- Creates natural pacing in combat

---

## Balance Considerations

### Minimum Viable Builds

**Tested Scenarios:**

**1. Balanced Build (Recommended for first-time players)**
- 12 HP, 12 ATK, 12 END, 8 EP, 4 Self-Discovery
- Can defeat Arlong with careful play
- Unlocks up to Front Lotus

**2. Glass Cannon**
- 8 HP, 16 ATK, 8 END, 8 EP, 8 Self-Discovery
- High risk, high reward
- Requires perfect execution

**3. Tank Build**
- 16 HP, 8 ATK, 16 END, 4 EP, 4 Self-Discovery
- Very safe but battles take longer
- May struggle with DPS requirements

### Victory Conditions

**Math Check:**
```
Arlong HP: 2500
Average damage needed per turn: ~83 (assuming 30-turn fight)

With G1 (1.8× ATK):
- 200 ATK with G1 = 360 effective ATK
- Against 600 END: (360-600)/2 = minimum damage (1)
- Requires END pierce skills to be viable

With skills + pierce:
- Front Lotus: 0.95× per hit, 30% pierce, 2 hits
- Can deal 150+ damage per use with proper build
```

**Conclusion:** Self-Discovery investment is CRITICAL for victory.

---

## Narrative Integration

### Story Beats Tied to Mechanics

1. **Bell-mere's Death** → Motivates entire training arc
2. **Eight Years** → Justifies dramatic power increase
3. **Gate Training** → Ancient scroll discovery grounds abilities
4. **Arlong's Phases** → Mirrors protagonist's growth narrative
5. **Defeat Loop** → Reinforces "training matters" theme
6. **Victory Ending** → Multiple epilogue scenes reward player investment

### Character Development Through Gameplay

- Training choices reflect player's interpretation of Keigan
- Combat style (aggressive vs. defensive) emerges naturally
- Skill selection creates unique combat identity
- Final battle performance determines ending quality

---

## Technical Choices

### Array-Based Data Structure

**Why Arrays Instead of Classes?**
- Simplicity for educational context
- Direct memory layout understanding
- No dynamic allocation complexity
- Easier to debug and visualize

**Trade-off:**
- Less flexible than OOP
- Fixed maximum sizes (MAX_SKILLS = 8)
- More manual bookkeeping

### Input Validation System

```cpp
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
```

**Design:**
- Prevents infinite loops on bad input
- Clears input buffer completely
- User-friendly error messages
- Never crashes on non-integer input

### Preview-Confirm System

- Shows stat changes before committing
- Prevents accidental training choices
- Reduces frustration
- Added after playtesting feedback

---

## Known Limitations & Future Improvements

### Current Limitations

1. **No save system** - Single session only
2. **Fixed skill tree** - No branching paths
3. **Single enemy** - No multiple boss fights
4. **Linear story** - One path through narrative
5. **No difficulty options** - Balanced for average players

### Potential Enhancements

1. **Save/Load System**
   - Save training progress
   - Resume battles mid-fight

2. **Multiple Endings**
   - Performance-based variants
   - Relationship scores with crew

3. **New Game+**
   - Keep one passive (Chain Handling OR Gate Mastery)
   - Harder Arlong scaling

4. **Additional Bosses**
   - Pre-Arlong minibosses
   - Post-game superbosses

5. **Equipment System**
   - Chains: +ATK, enables Chain skills
   - Bandages: +HP regen
   - Training weights: +stat gains

---

## Playtesting Insights

### Iteration 1 Issues
- Gates too weak (1.5× multiplier) → Increased to 1.8×/2.2×
- EP costs too high → Reduced and added Mastery
- Arlong too tanky → Reduced END from 800 to 600
- No skill preview → Added confirmation system

### Iteration 2 Issues
- Gate Crash too punishing → Added Mastery reduction
- No clear victory path → Added skill descriptions
- Training felt random → Added scaling formulas display

### Final Balance
- ~60% first-time victory rate with balanced build
- ~30% victory rate with unbalanced builds
- ~90% victory rate with optimal play

---

## Design Philosophy

**Core Principles:**
1. **Meaningful Choices** - Every training turn matters
2. **Strategic Depth** - Resource management over button mashing
3. **Fair Challenge** - Winnable with planning, not luck
4. **Narrative Integration** - Mechanics serve the story
5. **Transparency** - Players understand why they win/lose

**Target Audience:**
- Fans of text-based RPGs
- Players who enjoy strategic planning
- Anime combat system enthusiasts
- Those seeking ~60-90 minute gameplay sessions
