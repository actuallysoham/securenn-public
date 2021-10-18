#pragma once
#include "StableMatching.h"
using namespace std;

//PrefSuitor -> 2-d array for preference lists of all suitors. P_s[i][j] gives the preference of suitor i for reviewer j
//
//PrefReviewer Similar structure to P_s. GIves preferences of reviewers for suitors

//SuitorMatching -> 1-d array to store the matchings. Matching[i] gives the reviwer matched to suitor i. -1 indicates a suitor that hasn't been matched.

//IsMatched -> 2-D array to check if given party is matched. 
//IsMatched[index][SUITOR/REVIEWER]


//while i < |S| do 
    //for j ∈ S do
        //for k ∈ R do
            //[c] ← ([sr]j <? [ps(k)]j) × ([rs]k <? [pr(j)]k) 
            //[sr]j ← [c] [ps(k))]j : [sr]k  // [c] · ([x] − [y]) + [y]
            //[[rs]k ← [c] [pr(j)]k : [rs]k // [c] · ([x] − [y]) + [y]
        //end for 
    //end for
//    i++;
//end while 
//return [sr], [rs]

// S_r[] -> S_r[0] is the R which is currently matched with S_0
// R_s[] -> R_s[0] is the S which is currently matched with R_0
// P_s(k,j) -> 2D arraytruct Su

void ConditionalAssignment(vector<myType> choice, vector<myType> &x, vector<myType> &y, vector<myType> &z){
    vector<myType> z_temp(1);
    funcMatMulMPC(choice, (x − y), z_temp); // [c] · ([x] − [y]) 
    z = z_temp + y; // [z] ← [c] · ([x] − [y]) + [y]
}

void StableMatching(vector<myType> &PrefSuitor, vector<myType> &PrefReviewer, vector<myType> &Matching, int size){
    //initialize IsMatched
    int i=0;
    vector<myType> c1(1),c2(1),c(1);
    while(i<size){
        for(int j=0; j<size; j++){
            for(int k=0; k<size; k++){
                funcPrivateCompareMPC_2(SuitorMatching[k], PrefSuitor[k][j], c1);
                funcPrivateCompareMPC_2(ReviewerMatching[j], PrefReviewer[j][k] c2);
                funcMatMulMPC(c1, c2, c, 1,1,1,1,1);
                ConditionalAssignment(c, SuitorMatching[k], j, SuitorMatching[k]);
                ConditionalAssignment(c, ReviewerMatching[j], k, ReviewerMatching[j]);
            }
        }
    }

}
