#ifndef FCM_H
#define FCM_H

#include<map>
#include<string>

using namespace std;

class fcm{
	private:
		int k;
		double alfa;
	public:
		double distance;
		double modelEntropy;
		double textEntropy;
		int numChars;

		/**
		 * @brief Construct a new Finite-Context Models object.
		 * 
		 */
		fcm(){};

		/**
		 * @brief Construct a new Finite-Context Models object.
		 * 
		 * @param kIn order of the model.
		 * @param alfaIn smoothing parameter.
		 */
		fcm(int kIn, double alfaIn);

		/**
		 * @brief Builds a model by analysing the contents of the sample .txt file 
		 * named fileInName and prints it to the .txt file named fileOutName.
		 * @param model empty map to be loaded with the model
		 * @param fileInName name of the .txt file that contains the sample text to be analysed
		 * @param fileOutName name of the .txt file to print the model to.
		 */
		void modelBuilder(map<string,map<char,int>> &model, const char* fileInName, const char* fileOutName);
		
		/**
		 * @brief Loads a previously built model from the .txt file named fileInName.
		 * @param model empty map to be loaded with the model
		 * @param fileInName name of the .txt file that contains the previously built model
		 */
		void modelLoader(map<string,map<char,int>> &model, const char* fileInName);

		/**
		 * @brief Calculates the entropy of a model and saves it to the public attribute modelEntropy
		 * @param model map containing the model
		 */
		void calcEntropy(map<string,map<char,int>> &model);

		/**
		 * @brief Based on the provided model, estimates the entropy of the sample text 
		 * in the .txt file named fileInName
		 * @param model map containing the model
		 * @param fileInName name of the .txt file that contains the sample text to be analysed
		 */
		void estimateEntropy(map<string,map<char,int>> &model, const char* fileInName);
};

#endif
