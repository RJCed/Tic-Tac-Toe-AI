# Tic-Tac-Toe

A simple **terminal-based Tic-Tac-Toe game written in C**, where you play against an AI opponent using the **Minimax algorithm**.

> [!NOTE]
> **Interested specifically in the Minimax algorithm?**
>
> Check out [`ai.c`](ai.c). The Minimax implementation in this file was **handmade by me**.
>
> The rest of the game, including the game logic, UI, and other supporting code, was made with the help of AI. The purpose of this project was mainly for me to understand and implement the **Minimax algorithm** myself.


## How to Run

### Requirements

You need:

- GCC
- Make
- A Linux terminal

The game uses ANSI terminal features and is designed for Linux terminals.

### Run the Game

Clone the repository and enter the project directory:

```bash
git clone <repository-url>
cd <repository-folder>
```

Then run:

```bash
make run
```

That's it.

## How to Play

When the game starts, you can choose who goes first:

```text
1) You (X)
2) AI  (O)
```

When it is your turn, enter a move using the **column letter and row number**.

For example:

```text
A1
B2
C3
```

The board uses columns `A-C` and rows `1-3`.

You can enter `Q` at an input prompt to quit.

## How the AI Works

The AI uses the **Minimax algorithm** to choose its move.

It checks each available move, temporarily makes that move, and recursively considers what could happen afterward. The AI tries to **maximize its score**, while assuming the player will try to **minimize the score**.

The scores are:

```text
AI wins       →  1
Draw          →  0
Player wins   → -1
```

The AI then chooses the available move with the highest score. 

If you want to understand how the algorithm works, **`ai.c` is the main file to look at.**


## Future Improvements

Some things I would like to explore in the future:

- **Alpha-Beta Pruning** — improve Minimax by avoiding branches that do not need to be evaluated.
- **Larger Games** — apply Minimax to games with much larger game trees, such as chess.
- **Depth-Limited Minimax** — limit how far the AI searches when the complete game tree is too large.
- **Evaluation Functions** — instead of only evaluating wins and losses, give positions a score based on how good they are.
- **Improved UI Support** — Support for Windows and Mac.

## Author

**Arjay Cedigo**
