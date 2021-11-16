#pragma once
#include "StableMatching.h"
using namespace std;

/*
4
4 7 6 5
5 4 6 7
4 7 6 5
6 4 7 5
2 1 3 0
0 1 3 2
3 1 2 0
0 3 2 1
*/

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
    vector<myType> xy_diff(1);
    subtractVectors<myType>(x, y, xy_diff, 1);
    funcDotProductMPC(choice, xy_diff, z_temp, 1);
    addVectors<myType>(z_temp, y, z, 1);
}


void StableMatching(vector<vector<vector<myType>>> &PrefSuitor, vector<vector<vector<myType>>> &PrefReviewer, vector<vector<myType>> &Matching, int size){

    //initialize IsMatched
    int i=0, c_int;
    vector<myType> c1(1),c2(1),c(1), j_myType(1), k_myType(1), temp(1);
    //vector<vector<myType>> SuitorMatching;
    //vector<vector<myType>> ReviewerMatching;
    vector<int> SuitorMatching;
    vector<int> ReviewerMatching;
    for(int i=0; i<size; i++){
        //vector<myType> temp(1);
        //temp[0] = floatToMyType(0);
        //SuitorMatching.push_back(temp);
        //ReviewerMatching.push_back(temp);
	    SuitorMatching.push_back(-1);
        ReviewerMatching.push_back(-1);
    }
    // cout<< SuitorMatching[0][0]<<endl;

    while(i<size){
        for(int j=0; j<size; j++){
            for(int k=0; k<size; k++){
                
		if(PRIMARY){
                    //funcReconstruct2PC(SuitorMatching[j], 1, "Suitor Matching");
                    cout<<"Suitor Matching: "<<SuitorMatching[j]<<endl;
                    funcReconstruct2PC(PrefSuitor[j][k], 1, "PrefSuitor");
                }

		if (SuitorMatching[j] < 0){
			// initial case
            c1[0] = floatToMyType(0.5);
		}
        else{
             //funcPrivateCompareMPC_2(SuitorMatching[j], PrefSuitor[k][j], c1, 1, '<');
            funcPrivateCompareMPC_2(PrefSuitor[j][SuitorMatching[j]], PrefSuitor[k][j], c1, 1, '<');
        }      
                     
		if(PRIMARY){
                    funcReconstruct2PC(c1, 1, "(c1) Suitor Matching Pref < PrefSuitor");
                }

		if(PRIMARY){
                    //funcReconstruct2PC(ReviewerMatching[k], 1, "Reviewer Matching");
                    cout<<"Reviewer Matching: "<<ReviewerMatching[k]<<endl;
                    funcReconstruct2PC(PrefReviewer[k][j], 1, "PrefReviewer");
                }

		if (ReviewerMatching[k] < 0){
			// initial case
            c2[0] = floatToMyType(0.5);
		}
        else{
             //funcPrivateCompareMPC_2(ReviewerMatching[k], PrefReviewer[j][k], c2, 1, '<');
            funcPrivateCompareMPC_2(PrefReviewer[k][ReviewerMatching[k]], PrefReviewer[k][j], c2, 1, '<');
        }  
               

		if(PRIMARY){
                    funcReconstruct2PC(c2, 1, "(c2) Reviewer Matching Pref < PrefReviewer");
                }

                funcMatMulMPC(c1, c2, c, 1,1,1,1,1);

		if(PRIMARY){
                    funcReconstruct2PC(c, 1, "Choice = c1 x c2");
                }

                //k_myType[0] = floatToMyType(k/2.0);
                //j_myType[0] = floatToMyType(j/2.0);

                //ConditionalAssignment(c, SuitorMatching[k], j_myType, SuitorMatching[k]);
                //ConditionalAssignment(c, ReviewerMatching[j], k_myType, ReviewerMatching[j]);
		        //ConditionalAssignment(c, PrefSuitor[k][j], SuitorMatching[j], SuitorMatching[j]);
                //ConditionalAssignment(c, PrefReviewer[j][k], ReviewerMatching[k], ReviewerMatching[k]);
                
        //Reconstruct Conditional (c) and store in c_int
        if (partyNum == PARTY_B)
            sendVector<myType>(c, PARTY_A, 1);
        if (partyNum == PARTY_A)
            sendVector<myType>(c, PARTY_B, 1);

        if (partyNum == PARTY_A)
        {
            receiveVector<myType>(temp, PARTY_B, 1);
            addVectors<myType>(temp, c, temp, 1);
        }
        if (partyNum == PARTY_B)
        {
            receiveVector<myType>(temp, PARTY_A, 1);
            addVectors<myType>(temp, c, temp, 1);
        }
        if (partyNum == PARTY_B)
            sendVector<myType>(temp, PARTY_C, 1);
        if (partyNum == PARTY_C)
            receiveVector<myType>(temp, PARTY_B, 1);
        c_int = (static_cast<int64_t>(temp[0]))/(float)(1 << FLOAT_PRECISION);
        cout<<"c_int is: "<<c_int<<endl;

	if (c_int == 1){
			if (SuitorMatching[j] != -1) ReviewerMatching[SuitorMatching[j]] = -1;
			if (ReviewerMatching[k] != -1) SuitorMatching[ReviewerMatching[k]] = -1;
		}
        
        SuitorMatching[j] = (c_int)? k : SuitorMatching[j];
        ReviewerMatching[k] = (c_int)? j : ReviewerMatching[k];

	
		
		if(PRIMARY){
                //funcReconstruct2PC(SuitorMatching[j], 1, "SuitorMatching after Cond.Ass:");
                cout << "SuitorMatching after Conditional Assignment: "<<SuitorMatching[j]<<endl;
		        //funcReconstruct2PC(ReviewerMatching[k], 1, "ReviwerMatching after Cond.Ass:");
                cout << "ReviewerMatching after Conditional Assignment: "<<ReviewerMatching[k]<<endl;
                }
            }
        }
        i++;
    }
    if(PRIMARY){
        //funcReconstruct2PC(SuitorMatching[0], 1, "First Suitor Matching");
        //funcReconstruct2PC(SuitorMatching[1], 1, "Second Suitor Matching");
	for (int ind = 0; ind < size; ind ++){
		cout << "Suitor "<<ind<<" --> Reviewer "<<SuitorMatching[ind] + 1<<endl;

	}
        
    }


}
