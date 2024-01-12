/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Samsung
 *
 * Created on 20 de Janeiro de 2018, 23:23
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <list>

using namespace std;


int main() {
    int count = 0, valor = 0;
    const string nome;
    list<unsigned int, const string> ent; // entrevistador
    
    /*
    ent.push_back(0, "pergunta 1");
    ent.push_back(1, "pergunta 2");
    ent.push_back(2, "pergunta 3");
    */
    ent[0].push_back("pergunta 1");
    ent[1].push_back("pergunta 2");
    ent[2].push_back("pergutna 3");
    cout << ent[0].back() << ' ' << ent[1].back() << endl;
    
    
    return 0;
}

