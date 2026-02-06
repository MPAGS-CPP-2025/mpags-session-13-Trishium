#include "PlayfairCipher.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <iostream>

/**
 * \file PlayfairCipher.cpp
 * \brief Contains the implementation of the PlayfairCipher class
 */

PlayfairCipher::PlayfairCipher(const std::string& key)
{
    this->setKey(key);
}

void PlayfairCipher::setKey(const std::string& key)
{
    // Store the original key
    key_ = key;

    // Append the alphabet to the key
    key_ += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Make sure the key is upper case
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   ::toupper);

    // Remove non-alphabet characters
    key_.erase(std::remove_if(std::begin(key_), std::end(key_),
                              [](char c) { return !std::isalpha(c); }),
               std::end(key_));

    // Change J -> I
    std::transform(std::begin(key_), std::end(key_), std::begin(key_),
                   [](char c) { return (c == 'J') ? 'I' : c; });

    // Remove duplicated letters
    std::string lettersFound{""};
    auto detectDuplicates = [&](char c) {
        if (lettersFound.find(c) == std::string::npos) {
            lettersFound += c;
            return false;
        } else {
            return true;
        }
    };
    key_.erase(
        std::remove_if(std::begin(key_), std::end(key_), detectDuplicates),
        std::end(key_));

    // Store the coordinates of each letter
    // (at this point the key length must be equal to the square of the grid dimension)
    for (std::size_t i{0}; i < keyLength_; ++i) {
        std::size_t row{i / gridSize_};
        std::size_t column{i % gridSize_};

        auto coords = std::make_pair(row, column);

        charLookup_[key_[i]] = coords;
        coordLookup_[coords] = key_[i];
    }
}

std::string PlayfairCipher::applyCipher(const std::string& inputText,
                                        const CipherMode cipherMode) const
{
    int shift{};
    if (cipherMode == CipherMode::Encrypt){
        shift = 1;
    }
    else{
        shift = -1;
    }
    // Create the output string, initially a copy of the input text
    std::string outputText{inputText};
    std::transform(std::begin(outputText), std::end(outputText), std::begin(outputText),
                   ::toupper);
    outputText.erase(std::remove_if(std::begin(outputText), std::end(outputText),
                              [](char c) { return !std::isalpha(c); }),
               std::end(outputText));

    // Change J -> I
    std::transform(std::begin(outputText), std::end(outputText), std::begin(outputText),
                   [](char c) { return (c == 'J') ? 'I' : c; });

    // Find repeated characters and add an X (or a Q for repeated X's)
    std::vector<std::string> bigrams;
    for (uint i{0}; i < outputText.size(); i++){
         // if odd
        if (i == outputText.size()-1){
            bigrams.push_back(std::string{outputText[i]});
        }
        else if (outputText[i] == outputText[i+1] and outputText[i] != 'X'){
            bigrams.push_back(std::string{outputText[i]} + "X");
        } 
        else if (outputText[i] == outputText[i+1] and outputText[i] == 'X'){
            bigrams.push_back(std::string{outputText[i]} + "Q");
        }
        else{
            bigrams.push_back(outputText.substr(i,2));
            i++;
        }   
    }
    
    
    // If the size of the input is odd, add a trailing Z
    if (bigrams[bigrams.size()-1].size() == 1){
        if (bigrams[bigrams.size()-1][0] == 'Z'){
            bigrams[bigrams.size()-1] += "X";
        }
        else{
            bigrams[bigrams.size()-1] += "Z";
        }
    }
    
    // Loop over the input bigrams
    std::string newText{""};
    for (const auto& bigram : bigrams) {

    // - Find the coordinates in the grid for each bigram
   
        auto firstCoord = charLookup_.at(bigram[0]);
        auto secondCoord = charLookup_.at(bigram[1]);
        auto& [firstRow, firstColumn] {firstCoord};
        auto& [secondRow, secondColumn] {secondCoord};

    // - Apply the rules to these coords to get new coords
        if (firstRow == secondRow){
            firstColumn = (firstColumn + shift) % gridSize_;
            secondColumn = (secondColumn + shift) % gridSize_;
        }
        else if (firstColumn == secondColumn){
            firstRow = (firstRow + shift) % gridSize_;
            secondRow = (secondRow + shift) % gridSize_;
        }
        else{
            std::swap(firstColumn, secondColumn);
        }
    // - Find the letters associated with the new coords
        
        char firstLetter = coordLookup_.at(firstCoord);
        char secondLetter = coordLookup_.at(secondCoord);
    // - Make the replacements
        newText += firstLetter;
        newText += secondLetter;
    }
    // Return the output text
    outputText = newText;
    return outputText;
}
