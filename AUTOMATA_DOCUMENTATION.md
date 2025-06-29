# Finite Automata for Lexical Analyzer

This document provides detailed explanations of the finite automata (DFA/NFA) designed for the lexical analyzer project.

## Project Overview

This lexical analyzer recognizes the following token types:
1. **Identifiers**: Must start with letter or underscore and contain at least one underscore
2. **Numbers**: Integers, decimals, and scientific notation [+-]?(D+)(\.D+)?(E[+-]?D+)?
3. **Operators**: !=, <>, =:=, ==, *, +, /, -, >>, <<, ++, =+, &&, ||, =>, =<, %, :, ::, --
4. **Punctuations**: [, {, <, >, }, ], (, ), ,, ;
5. **Keywords**: loop, agar, magar, asm, else, new, this, auto, enum, operator, throw, bool, explicit, private, true, break, export, protected, try, case, extern, public, typedef, catch, false, register, typeid, char, float, typename, class, for, return, union, const, friend, short, unsigned, goto, signed, using, continue, if, sizeof, virtual, default, inline, static, void, delete, int, volatile, do, long, struct, double, mutable, switch, while, namespace

## Automata Descriptions

### 1. Identifier DFA (`identifier_dfa.png`)

**Purpose**: Recognizes valid identifiers that must contain at least one underscore.

**States**:
- `q0` (start): Initial state
- `q1` (letter read): Letter read but no underscore yet
- `q2` (underscore seen): At least one underscore encountered (accepting)
- `q3` (continue): Continue reading after underscore (accepting)

**Transitions**:
- From `q0`: letter → `q1`, underscore → `q2`
- From `q1`: letter/digit → `q1`, underscore → `q2`
- From `q2`: letter/digit/underscore → `q3`
- From `q3`: letter/digit/underscore → `q3`

**Examples**: `_rate2`, `_rate_`, `rat1e_2` (valid), `rat1e2` (invalid - no underscore)

### 2. Number DFA (`number_dfa.png`)

**Purpose**: Recognizes integers, floating-point numbers, and scientific notation.

**States**:
- `q0` (start): Initial state
- `q1` (sign): Optional sign read
- `q2` (integer): Integer part (accepting)
- `q3` (decimal point): Decimal point read
- `q4` (fraction): Fractional part (accepting)
- `q5` (E): Exponent marker read
- `q6` (exp sign): Exponent sign read
- `q7` (exponent): Exponent digits (accepting)

**Pattern**: [+-]?(D+)(\.D+)?(E[+-]?D+)?

**Examples**: `123`, `+45.67`, `-0.5E-2`, `3.43433E+13`

### 3. Operator NFA (`operator_nfa.png`)

**Purpose**: Recognizes all single and multi-character operators using NFA approach.

**Key Features**:
- Handles longest match principle
- Recognizes both single-char operators (*, +, /, -, %, :) and multi-char operators
- Multi-character operators: !=, <>, =:=, ==, >>, <<, ++, =+, &&, ||, =>, =<, ::, --

**Strategy**: Uses intermediate states to build multi-character operators step by step.

### 4. Punctuation DFA (`punctuation_dfa.png`)

**Purpose**: Simple DFA for punctuation characters.

**Recognized Characters**: [, {, <, >, }, ], (, ), ,, ;

**Structure**: Single transition from start state to accepting state for any punctuation character.

### 5. Keyword DFA (`keyword_dfa.png`)

**Purpose**: Simplified representation of keyword recognition.

**Strategy**: 
- Read sequence of letters/digits
- At end of word, check if sequence matches any keyword in the predefined list
- If match: accept as keyword
- If no match: reject (would be handled as identifier or error)

### 6. Complete Lexer DFA (`complete_lexer_dfa.png`)

**Purpose**: High-level overview of the entire lexical analysis process.

**Shows**: How the lexer dispatches to different token recognition automata based on the first character(s) of input.

## Implementation Notes

1. **Table-Driven Approach**: The actual implementation uses transition tables derived from these automata.

2. **Maximal Munch**: The lexer always tries to match the longest possible token.

3. **Error Handling**: Invalid sequences that don't match any automaton are reported as errors.

4. **Keyword vs Identifier**: Identifiers are first recognized by the identifier DFA, then checked against the keyword list.

5. **Comment Handling**: The lexer skips single-line (//) and multi-line (/* */) comments before token recognition.

## Usage in Code

The C++ implementation in `main.cpp` implements these automata using:
- State machines for number recognition
- Character-by-character processing
- Lookahead for multi-character operators
- Keyword lookup table

Each automaton corresponds to specific sections in the `analyze()` method of the `LexicalAnalyzer` class.
