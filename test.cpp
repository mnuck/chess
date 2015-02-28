#include <algorithm>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "Board.h"
#include "test.h"

bool test()
{ 
    bool result = true;

    std::vector<std::string> filenames;

    // get list of files in directory
    char currentPath[FILENAME_MAX];
    getcwd(currentPath, sizeof(currentPath));
    
    std::string path(currentPath);
    path += "/tests/";
    

    struct dirent* ent;
    DIR* dir = opendir(path.c_str());
    if (dir != nullptr)
    {
        while ((ent = readdir(dir)) != nullptr)
        {
            std::string filename(ent->d_name);
            if (filename != ".." && filename != ".")
                filenames.push_back("tests/" + filename);
        }
        closedir(dir);
    }
    
    for (std::string& filename: filenames)
    {
        std::cout << "Now Testing: " << filename << std::endl;
        
        std::ifstream testFile(filename);
        BixNix::Board board = BixNix::Board::parse(testFile);
        int source, target;
        size_t whiteMoveCount, blackMoveCount;
        std::vector<BixNix::Move> whiteTrueMoves;
        std::vector<BixNix::Move> blackTrueMoves;

        testFile >> whiteMoveCount;
        for (size_t i = 0; i < whiteMoveCount; ++i)
        {
            testFile >> source >> target;
            whiteTrueMoves.push_back(BixNix::Move(source, target));
        }
        testFile >> blackMoveCount;
        for (size_t i = 0; i < blackMoveCount; ++i)
        {
            testFile >> source >> target;
            blackTrueMoves.push_back(BixNix::Move(source, target));            
        }

        
        testFile.close();        

        std::vector<BixNix::Move> whiteGeneratedMoves = board.getMoves(BixNix::White);
        std::vector<BixNix::Move> blackGeneratedMoves = board.getMoves(BixNix::Black);
        
        // I know. I don't care. n is small.
        for (BixNix::Move& m: whiteGeneratedMoves)
        {
            auto it = std::find(
                whiteTrueMoves.begin(),
                whiteTrueMoves.end(),
                m);
            if (it == whiteTrueMoves.end())
            {
                std::cout << m << " generated in error" << std::endl;
                result = false;
            }
        }
        for (BixNix::Move& m: whiteTrueMoves)
        {
            auto it = std::find(
                whiteGeneratedMoves.begin(),
                whiteGeneratedMoves.end(),
                m);
            if (it == whiteGeneratedMoves.end())
            {
                std::cout << m << " not generated but should be" << std::endl;
                result = false;
            }
        }
        for (BixNix::Move& m: blackGeneratedMoves)
        {
            auto it = std::find(
                blackTrueMoves.begin(),
                blackTrueMoves.end(),
                m);
            if (it == blackTrueMoves.end())
            {
                std::cout << m << " generated in error" << std::endl;
                result = false;
            }
        }
        for (BixNix::Move& m: blackTrueMoves)
        {
            auto it = std::find(
                blackGeneratedMoves.begin(),
                blackGeneratedMoves.end(),
                m);
            if (it == blackGeneratedMoves.end())
            {
                std::cout << m << " not generated but should be" << std::endl;
                result = false;
            }
        }
    }

    return result;
}
