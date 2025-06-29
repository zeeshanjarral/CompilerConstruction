#!/usr/bin/env python3

import graphviz
import os

def create_identifier_dfa():
    """
    Create DFA for identifiers: Start with _ or letter, must contain at least one "_"
    """
    dot = graphviz.Digraph(comment='Identifier DFA')
    dot.attr(rankdir='LR')
    dot.attr('node', shape='circle')
    
    dot.node('q0', 'q0\\n(start)', shape='circle')
    dot.node('q1', 'q1\\n(letter read)', shape='circle')
    dot.node('q2', 'q2\\n(underscore seen)', shape='doublecircle')
    dot.node('q3', 'q3\\n(continue)', shape='doublecircle')
    
    dot.edge('q0', 'q1', 'letter')
    dot.edge('q0', 'q2', '_')
    dot.edge('q1', 'q1', 'letter, digit')
    dot.edge('q1', 'q2', '_')
    dot.edge('q2', 'q3', 'letter, digit, _')
    dot.edge('q3', 'q3', 'letter, digit, _')
    
    return dot

def create_number_dfa():
    """
    Create DFA for numbers: [+-]?(D+)(\.D+)?(E[+-]?D+)?
    """
    dot = graphviz.Digraph(comment='Number DFA')
    dot.attr(rankdir='LR')
    dot.attr('node', shape='circle')
    
    dot.node('q0', 'q0\\n(start)', shape='circle')
    dot.node('q1', 'q1\\n(sign)', shape='circle')
    dot.node('q2', 'q2\\n(integer)', shape='doublecircle')
    dot.node('q3', 'q3\\n(decimal point)', shape='circle')
    dot.node('q4', 'q4\\n(fraction)', shape='doublecircle')
    dot.node('q5', 'q5\\n(E)', shape='circle')
    dot.node('q6', 'q6\\n(exp sign)', shape='circle')
    dot.node('q7', 'q7\\n(exponent)', shape='doublecircle')
    
    dot.edge('q0', 'q1', '+, -')
    dot.edge('q0', 'q2', 'digit')
    dot.edge('q1', 'q2', 'digit')
    dot.edge('q2', 'q2', 'digit')
    dot.edge('q2', 'q3', '.')
    dot.edge('q2', 'q5', 'E, e')
    dot.edge('q3', 'q4', 'digit')
    dot.edge('q4', 'q4', 'digit')
    dot.edge('q4', 'q5', 'E, e')
    dot.edge('q5', 'q6', '+, -')
    dot.edge('q5', 'q7', 'digit')
    dot.edge('q6', 'q7', 'digit')
    dot.edge('q7', 'q7', 'digit')
    
    return dot

def create_operator_nfa():
    """
    Create NFA for operators - simplified version to avoid graphviz issues
    """
    dot = graphviz.Digraph(comment='Operator NFA')
    dot.attr(rankdir='LR')
    dot.attr('node', shape='circle')
    
    dot.node('q0', 'q0\\n(start)', shape='circle')
    
    dot.node('star', 'STAR\\n(*)', shape='doublecircle')
    dot.node('plus', 'PLUS\\n(+)', shape='doublecircle')
    dot.node('slash', 'SLASH\\n(/)', shape='doublecircle')
    dot.node('minus', 'MINUS\\n(-)', shape='doublecircle')
    dot.node('percent', 'PERCENT\\n(%)', shape='doublecircle')
    dot.node('colon', 'COLON\\n(:)', shape='doublecircle')
    
    dot.node('ne', 'NE\\n(!=)', shape='doublecircle')
    dot.node('ne2', 'NE2\\n(<>)', shape='doublecircle')
    dot.node('assign', 'ASSIGN\\n(=:=)', shape='doublecircle')
    dot.node('eq', 'EQ\\n(==)', shape='doublecircle')
    dot.node('rshift', 'RSHIFT\\n(>>)', shape='doublecircle')
    dot.node('lshift', 'LSHIFT\\n(<<)', shape='doublecircle')
    dot.node('inc', 'INC\\n(++)', shape='doublecircle')
    dot.node('eplus', 'EPLUS\\n(=+)', shape='doublecircle')
    dot.node('and_op', 'AND\\n(&&)', shape='doublecircle')
    dot.node('or_op', 'OR\\n(||)', shape='doublecircle')
    dot.node('ge', 'GE\\n(=>)', shape='doublecircle')
    dot.node('le', 'LE\\n(=<)', shape='doublecircle')
    dot.node('scope', 'SCOPE\\n(::)', shape='doublecircle')
    dot.node('dec', 'DEC\\n(--)', shape='doublecircle')
    
    dot.node('excl', 'q_excl', shape='circle')
    dot.node('less', 'q_less', shape='circle')
    dot.node('equal', 'q_equal', shape='circle')
    dot.node('greater', 'q_greater', shape='circle')
    dot.node('amp', 'q_amp', shape='circle')
    dot.node('pipe', 'q_pipe', shape='circle')
    dot.node('eq_colon', 'q_eq_colon', shape='circle')
    
    dot.edge('q0', 'star', '*')
    dot.edge('q0', 'plus', '+')
    dot.edge('q0', 'slash', '/')
    dot.edge('q0', 'minus', '-')
    dot.edge('q0', 'percent', '%')
    dot.edge('q0', 'colon', ':')
    
    dot.edge('q0', 'excl', '!')
    dot.edge('excl', 'ne', '=')
    
    dot.edge('q0', 'less', '<')
    dot.edge('less', 'ne2', '>')
    dot.edge('less', 'lshift', '<')
    
    dot.edge('q0', 'equal', '=')
    dot.edge('equal', 'eq', '=')
    dot.edge('equal', 'eplus', '+')
    dot.edge('equal', 'ge', '>')
    dot.edge('equal', 'le', '<')
    dot.edge('equal', 'eq_colon', ':')
    dot.edge('eq_colon', 'assign', '=')
    
    dot.edge('q0', 'greater', '>')
    dot.edge('greater', 'rshift', '>')
    
    dot.edge('plus', 'inc', '+')
    dot.edge('minus', 'dec', '-')
    
    dot.edge('q0', 'amp', '&')
    dot.edge('amp', 'and_op', '&')
    
    dot.edge('q0', 'pipe', '|')
    dot.edge('pipe', 'or_op', '|')
    
    dot.edge('colon', 'scope', ':')
    
    return dot

def create_punctuation_dfa():
    """
    Create DFA for punctuations: [, {, <, >, }, ]
    """
    dot = graphviz.Digraph(comment='Punctuation DFA')
    dot.attr(rankdir='LR')
    dot.attr('node', shape='circle')
    
    dot.node('q0', 'q0\\n(start)', shape='circle')
    dot.node('q1', 'PUNCT', shape='doublecircle')
    
    dot.edge('q0', 'q1', 'punctuation\\nchars')
    
    return dot

def create_keyword_dfa():
    """
    Create DFA for keywords (simplified representation)
    """
    dot = graphviz.Digraph(comment='Keyword DFA')
    dot.attr(rankdir='LR')
    dot.attr('node', shape='circle')
    
    dot.node('q0', 'q0\\n(start)', shape='circle')
    dot.node('q1', 'q1\\n(letter)', shape='circle')
    dot.node('q2', 'KEYWORD', shape='doublecircle')
    dot.node('q3', 'NOT_KEYWORD', shape='circle')
    
    dot.edge('q0', 'q1', 'letter')
    dot.edge('q1', 'q1', 'letter, digit')
    dot.edge('q1', 'q2', 'end of word\\n(if in keyword list)')
    dot.edge('q1', 'q3', 'end of word\\n(if not in keyword list)')
    
    return dot

def create_complete_lexer_dfa():
    """
    Create a high-level DFA showing the complete lexical analyzer structure
    """
    dot = graphviz.Digraph(comment='Complete Lexical Analyzer DFA')
    dot.attr(rankdir='LR')
    dot.attr('node', shape='circle')
    
    dot.node('START', 'START', shape='circle')
    dot.node('WHITESPACE', 'WHITESPACE', shape='circle')
    dot.node('COMMENT', 'COMMENT', shape='circle')
    dot.node('IDENTIFIER', 'IDENTIFIER', shape='doublecircle')
    dot.node('NUMBER', 'NUMBER', shape='doublecircle')
    dot.node('OPERATOR', 'OPERATOR', shape='doublecircle')
    dot.node('PUNCTUATION', 'PUNCTUATION', shape='doublecircle')
    dot.node('KEYWORD', 'KEYWORD', shape='doublecircle')
    dot.node('ERROR', 'ERROR', shape='doublecircle')
    
    dot.edge('START', 'WHITESPACE', 'space, tab, newline')
    dot.edge('START', 'COMMENT', '/, *')
    dot.edge('START', 'IDENTIFIER', 'letter, _')
    dot.edge('START', 'NUMBER', 'digit, +, -')
    dot.edge('START', 'OPERATOR', 'operator chars')
    dot.edge('START', 'PUNCTUATION', 'punctuation chars')
    dot.edge('START', 'ERROR', 'invalid char')
    
    dot.edge('WHITESPACE', 'START', 'end whitespace')
    dot.edge('COMMENT', 'START', 'end comment')
    dot.edge('IDENTIFIER', 'KEYWORD', 'if keyword')
    
    return dot

def main():
    """Generate all automata diagrams"""
    
    os.makedirs('automata_diagrams', exist_ok=True)
    
    diagrams = [
        ('identifier_dfa', create_identifier_dfa()),
        ('number_dfa', create_number_dfa()),
        ('operator_nfa', create_operator_nfa()),
        ('punctuation_dfa', create_punctuation_dfa()),
        ('keyword_dfa', create_keyword_dfa()),
        ('complete_lexer_dfa', create_complete_lexer_dfa())
    ]
    
    for name, diagram in diagrams:
        diagram.render(f'automata_diagrams/{name}', format='png', cleanup=True)
        print(f"Generated: automata_diagrams/{name}.png")
    
    print("\\nAll automata diagrams have been generated successfully!")
    print("Files created:")
    for name, _ in diagrams:
        print(f"  - automata_diagrams/{name}.png")

if __name__ == "__main__":
    main()
