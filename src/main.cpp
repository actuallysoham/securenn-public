
#include <iostream>
#include <string>
#include "secondary.h"
#include "connect.h"
#include "AESObject.h"
#include "NeuralNetConfig.h"
#include "NeuralNetwork.h"
#include "Functionalities.h"
#include "StableMatching.h"


using namespace std;
extern int partyNum;
int NUM_OF_PARTIES;	


AESObject* aes_common;
AESObject* aes_indep;
AESObject* aes_a_1;
AESObject* aes_a_2;
AESObject* aes_b_1;
AESObject* aes_b_2;
AESObject* aes_c_1;
ParallelAESObject* aes_parallel;



int main(int argc, char** argv)
{

/****************************** PREPROCESSING ******************************/ 
	parseInputs(argc, argv);
	string whichNetwork = "No Network";
	NeuralNetConfig* config = new NeuralNetConfig(NUM_ITERATIONS);


/****************************** SELECT NETWORK ******************************/ 
	//MINIONN, Network-D in GAZELLE
	whichNetwork = "MiniONN/GAZELLE-D";
	CNNConfig* l0 = new CNNConfig(16,1,5,5,MINI_BATCH_SIZE,28,28,2,2);
	CNNConfig* l1 = new CNNConfig(16,16,5,5,MINI_BATCH_SIZE,12,12,2,2);
	FCConfig* l2 = new FCConfig(MINI_BATCH_SIZE, 256, 100);
	FCConfig* l3 = new FCConfig(MINI_BATCH_SIZE, 100, 10);
	config->addLayer(l0);
	config->addLayer(l1);
	config->addLayer(l2);
	config->addLayer(l3);



	//LeNet
	// whichNetwork = "LeNet";
	// CNNConfig* l0 = new CNNConfig(20,1,5,5,MINI_BATCH_SIZE,28,28,2,2);
	// CNNConfig* l1 = new CNNConfig(50,20,5,5,MINI_BATCH_SIZE,12,12,2,2);
	// FCConfig* l2 = new FCConfig(MINI_BATCH_SIZE, 800, 500);
	// FCConfig* l3 = new FCConfig(MINI_BATCH_SIZE, 500, 10);
	// config->addLayer(l0);
	// config->addLayer(l1);
	// config->addLayer(l2);
	// config->addLayer(l3);

	//SecureML
	// whichNetwork = "SecureML";
	// FCConfig* l0 = new FCConfig(MINI_BATCH_SIZE, LAYER0, LAYER1); 
	// FCConfig* l1 = new FCConfig(MINI_BATCH_SIZE, LAYER1, LAYER2); 
	// FCConfig* l2 = new FCConfig(MINI_BATCH_SIZE, LAYER2, LAST_LAYER_SIZE); 
	// config->addLayer(l0);
	// config->addLayer(l1);
	// config->addLayer(l2);

	//Chameleon
	// whichNetwork = "Sarda";
	// ChameleonCNNConfig* l0 = new ChameleonCNNConfig(5,1,5,5,MINI_BATCH_SIZE,28,28,2,2);
	// FCConfig* l1 = new FCConfig(MINI_BATCH_SIZE, 980, 100);
	// FCConfig* l2 = new FCConfig(MINI_BATCH_SIZE, 100, 10);
	// config->addLayer(l0);
	// config->addLayer(l1);
	// config->addLayer(l2);

	
	config->checkNetwork();
	NeuralNetwork* network = new NeuralNetwork(config);


/****************************** AES SETUP and SYNC ******************************/ 
	aes_indep = new AESObject(argv[4]);
	aes_common = new AESObject(argv[5]);
	aes_a_1 = new AESObject("files/keyD");
	aes_a_2 = new AESObject("files/keyD");
	aes_b_1 = new AESObject("files/keyD");
	aes_b_2 = new AESObject("files/keyD");
	aes_c_1 = new AESObject("files/keyD");
	aes_parallel = new ParallelAESObject(argv[5]);

	if (!STANDALONE)
	{
		initializeCommunication(argv[3], partyNum);
		synchronize(2000000);	
	}

	if (PARALLEL)
		aes_parallel->precompute();


/****************************** RUN NETWORK/BENCHMARKS ******************************/ 
	start_m();

	// whichNetwork = "Mat-Mul";
	// testMatMul(784, 128, 10, NUM_ITERATIONS);
	// testMatMul(1, 500, 100, NUM_ITERATIONS);
	// testMatMul(1, 100, 1, NUM_ITERATIONS);

	// whichNetwork = "Convolution";
	// testConvolution(28, 28, 5, 5, 1, 20, NUM_ITERATIONS);
	// testConvolution(28, 28, 3, 3, 1, 20, NUM_ITERATIONS);
	// testConvolution(8, 8, 5, 5, 16, 50, NUM_ITERATIONS);

	// whichNetwork = "Relu";
	// testRelu(128, 128, NUM_ITERATIONS);
	// testRelu(576, 20, NUM_ITERATIONS);
	// testRelu(64, 16, NUM_ITERATIONS);

	// whichNetwork = "ReluPrime";
	// testReluPrime(128, 128, NUM_ITERATIONS);
	// testReluPrime(576, 20, NUM_ITERATIONS);
	// testReluPrime(64, 16, NUM_ITERATIONS);

	// whichNetwork = "MaxPool";
	// testMaxPool(24, 24, 2, 2, 20, NUM_ITERATIONS);
	// testMaxPool(24, 24, 2, 2, 16, NUM_ITERATIONS);
	// testMaxPool(8, 8, 4, 4, 50, NUM_ITERATIONS);

	// whichNetwork = "MaxPoolDerivative";
	// testMaxPoolDerivative(24, 24, 2, 2, 20, NUM_ITERATIONS);
	// testMaxPoolDerivative(24, 24, 2, 2, 16, NUM_ITERATIONS);
	// testMaxPoolDerivative(8, 8, 4, 4, 50, NUM_ITERATIONS);

	whichNetwork += " train";
	train(network, config);

	// whichNetwork += " test";
	// test(network);



	end_m(whichNetwork);
	cout << "----------------------------------" << endl;  	
	cout << NUM_OF_PARTIES << "PC code, P" << partyNum << endl;
	cout << NUM_ITERATIONS << " iterations, " << whichNetwork << ", batch size " << MINI_BATCH_SIZE << endl;
	cout << "----------------------------------" << endl << endl;  

/****************************** TESTSING ******************************/

	int size = 1;
	
	vector<myType> a(size),b(size), c(size);

	a[0] = floatToMyType(1);
	b[0] = floatToMyType(1);

	for(int i=0; i<10; i++){
		funcPrivateCompareMPC_2(a,b,c,size,'=');

		if(PRIMARY){
			funcReconstruct2PC(c, size, "Output of comparison");
		}
	}


    cout<<"=================================="<<endl;
    cout<<"===== SECURE STABLE MATCHING ====="<<endl;
    cout<<"=================================="<<endl;
    
    vector<vector<vector<myType>>> PrefSuitor, PrefReviewer;

    vector<vector<myType>> PreferenceList;
    vector<vector<myType>> Matching;

    vector<myType> preference(1);

//////////////////// Parsing Input (TODO: refactor as a function)

    int numSuitors;    
    float input;
    cout<<"Enter number of suitors/ reviewers: ";
    cin>>numSuitors;
    cout<<"Number of suitors/ reviewers: "<<numSuitors<<endl;

    for (int i = 0; i < numSuitors; i ++){
   	 for (int j = 0; j < numSuitors; j++){
   		 cout<<"Suitor "<<i<<"'s preference for Reviewer "<<j<<" : ";
   		 cin>>input; //pref of S[i] for R[j]    
   		 preference[0] = floatToMyType(input);
   		 PreferenceList.push_back(preference);
   	 }

   	 PrefSuitor.push_back(PreferenceList);    
    }

    for (int i = 0; i < numSuitors; i ++){
   	 for (int j = 0; j < numSuitors; j++){
   		 cout<<"Reviewer "<<i<<"'s preference for Suitor "<<j<<" : ";
   		 cin>>input; //pref of R[i] for S[j]
   		 preference[0] = floatToMyType(input);
   		 PreferenceList.push_back(preference);    
   	 }

   	 PrefReviewer.push_back(PreferenceList);    
    }

//////////////////// End of Parsing Input

/*
    if(PRIMARY){
    	funcReconstruct2PC(PrefSuitor[0][0], 1, "PrefSuitor first element");
    	funcReconstruct2PC(PrefReviewer[0][0], 1, "PrefReviewer first element");
    	funcReconstruct2PC(PrefSuitor[0][1], 1, "PrefSuitor second element");
    	funcReconstruct2PC(PrefReviewer[0][1], 1, "PrefReviewer second element");
    	funcReconstruct2PC(PrefSuitor[1][0], 1, "PrefSuitor third element");
    	funcReconstruct2PC(PrefReviewer[1][0], 1, "PrefReviewer third element");
    	funcReconstruct2PC(PrefSuitor[0][1], 1, "PrefSuitor fourth element");
    	funcReconstruct2PC(PrefReviewer[0][1], 1, "PrefReviewer fourth element");
	}

*/
    preference[0] = floatToMyType(0);
    PreferenceList.push_back(preference);
    preference[0] = floatToMyType(0.5);
    PreferenceList.push_back(preference);

    PrefSuitor.push_back(PreferenceList);
    PrefReviewer.push_back(PreferenceList);

    preference[0] = floatToMyType(0.5);
    PreferenceList.push_back(preference);
    preference[0] = floatToMyType(0);
    PreferenceList.push_back(preference);

    PrefSuitor.push_back(PreferenceList);
    PrefReviewer.push_back(PreferenceList);

    if(PRIMARY){
    	funcReconstruct2PC(PrefSuitor[0][0], 1, "PrefSuitor first element");
    	funcReconstruct2PC(PrefReviewer[0][0], 1, "PrefReviewer first element");
    	funcReconstruct2PC(PrefSuitor[0][1], 1, "PrefSuitor second element");
    	funcReconstruct2PC(PrefReviewer[0][1], 1, "PrefReviewer second element");
    	funcReconstruct2PC(PrefSuitor[1][0], 1, "PrefSuitor third element");
    	funcReconstruct2PC(PrefReviewer[1][0], 1, "PrefReviewer third element");
    	funcReconstruct2PC(PrefSuitor[0][1], 1, "PrefSuitor fourth element");
    	funcReconstruct2PC(PrefReviewer[0][1], 1, "PrefReviewer fourth element");
	}


    cout<<"Entering Stable matching"<<endl;
    StableMatching(PrefSuitor, PrefReviewer, Matching, 2);


/****************************** CLEAN-UP ******************************/ 
	delete aes_common;
	delete aes_indep;
	delete aes_a_1;
	delete aes_a_2;
	delete aes_b_1;
	delete aes_b_2;
	delete aes_c_1;
	delete aes_parallel;
	delete config;
	delete l0;
	delete l1;
	delete l2;
	// delete l3;
	delete network;
	if (partyNum != PARTY_S)
		deleteObjects();

	return 0;
}

