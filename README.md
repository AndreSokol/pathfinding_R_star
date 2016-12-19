# Pathfinding algorithms (R\*)
Course project for HSE Computer Sceince Faculty

**Student:** Andrey Sokolov    
**Group:** БПМИ156    
**Mentor:** [Yakovlev K. S.](//github.com/konstantin-yakovlev)


### Contents

1. [Introduction](#introduction)    
2. [Tools used](#tools-used)    
3. [Work plan](#work-plan)    


## Introduction

Problem of finding path comes in different types of practical use, such as, for example, artifical intelligence systems. One important aspect of the problem is resource limitations - increase of map size causes dramatic growth of amount of memory and time required and in real life search agents possess very little of them.

History of this problem can be started from the article *'A note on two problems in connexion with graphs'* written by E. W. Dijkstra in 1959. After that big milestones were A\* and Jump-Point search algorithms improved performance a lot (these algorithms were presented and researched during the [summer internship](//github.com/AndreSokol/pathfinding_algorithms)).

In this work I will research the R\*-search also known as randomized A\*-search and try to improve its performance.

## Tools used

This work is based on code written for ISA RAS researches in order to achieve compability with their huge set of pre-generated tests. That is the main reason to choose the following tools:

**Programming language:** C++, XML (for input files)    
**IDE:** QtCreator 3.6.1    
**Compiler:** qmake + MinGW         
**External library:** TinyXML 2 (for parsing input files)    

## Work plan

