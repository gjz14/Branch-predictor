#include <stdio.h>
#include <string.h>
#include <math.h>
#include "predictor.h"


// perceptron number
#define p_NUM 512
//bits of history 
#define p_HistorySize 19

 // The number of bits each weight can use.
#define p_WeightSize 8

#define p_MASK_PC(x) ((x * p_HistorySize) % p_NUM)

int16_t p_W[p_NUM][p_HistorySize + 1];
int16_t p_gHistory[p_HistorySize];
static const int32_t p_train_theta = (1.93 * p_HistorySize + 14);
uint8_t p_HistoryStart = NOTTAKEN;
uint8_t train_flag = 0;
int weightsize = 1 << (p_WeightSize - 1);



//======================Perceptron Predictor==================================
int sign(int val) {
    return (val>0)?1:-1;
 }



void init_perceptron(){
  
  memset(p_W, 0, sizeof(int16_t) * p_NUM * (p_HistorySize + 1));
  memset(p_gHistory, 0, sizeof(uint16_t) * p_HistorySize);
}



uint8_t make_perceptron_prediction(uint32_t pc){
  uint32_t index = p_MASK_PC(pc);
  int16_t out = p_W[index][0];

  for(int i = 1 ; i <= p_HistorySize ; i++){
    out += sign(p_gHistory[i-1]) *  p_W[index][i] ;
  }

  p_HistoryStart = (out >= 0) ? TAKEN : NOTTAKEN;

  train_flag = (abs(out) < p_train_theta ) ? 1 : 0;

  return p_HistoryStart;
}

void train_perceptron(uint32_t pc, uint8_t outcome){

  uint32_t index = p_MASK_PC(pc);
  // need train
  if((p_HistoryStart != outcome) || train_flag){

	int res = p_W[index][0] + sign(outcome);
	if( abs(res) < weightsize){
		p_W[index][0] = res;
	}
    for(int i = 1 ; i <= p_HistorySize ; i++){
	  res = p_W[index][i] + sign(outcome)*sign(p_gHistory[i-1]);
	  if(abs(res)<weightsize){
		p_W[index][i] = res;
	  }
    }

  }

  for(int i = p_HistorySize - 1; i > 0 ; i--){
    p_gHistory[i] = p_gHistory[i-1];
  }
  p_gHistory[0] = outcome;

}