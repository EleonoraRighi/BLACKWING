#pragma once


//default position
static const int default_seq1[] = {DEFAULT_ANGLE_1};
static const int default_seq2[] = {DEFAULT_ANGLE_2}; 
static const int default_seq3[] = {DEFAULT_ANGLE_3};
static const int default_seq4[] = {DEFAULT_ANGLE_4};



//ANGER
//annoiance sequences
static const int annoyance_seq1[]={90,70,90};
static const int annoyance_seq2[]={DEFAULT_ANGLE_2+8,DEFAULT_ANGLE_2-6, DEFAULT_ANGLE_2+8};
static const int annoyance_seq3[]={180,90,0};//
static const int annoyance_seq4[]={DEFAULT_ANGLE_4,100,100,100,0,0,0};

//anger sequences
static const int anger_seq1[]={90,50,90,70,90,70,90,50,90,70,90,70,90,50,90,90,90};
static const int anger_seq2[]={DEFAULT_ANGLE_2+9};
static const int anger_seq3[]={DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3, DEFAULT_ANGLE_3+20,DEFAULT_ANGLE_3+20,DEFAULT_ANGLE_3+20,DEFAULT_ANGLE_3+20,DEFAULT_ANGLE_3-20,DEFAULT_ANGLE_3-20,DEFAULT_ANGLE_3-20,DEFAULT_ANGLE_3-20};
static const int anger_seq4[]={DEFAULT_ANGLE_4+10};
//rage sequences
static const int rage_seq1[]={90,30,90,50,90,50,90,30,90,50,90,50,90,30,90,50,90,90,30,90,50,90,50};
static const int rage_seq2[]={DEFAULT_ANGLE_2+10, DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2+10, DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2+10, DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2+10, DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10,DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10};
static const int rage_seq3[]={DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3+60,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3+20,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3};
static const int rage_seq4[]={DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+5,DEFAULT_ANGLE_4-5,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+5,DEFAULT_ANGLE_4-5,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+5,DEFAULT_ANGLE_4-5,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+5,DEFAULT_ANGLE_4-5,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+5,DEFAULT_ANGLE_4-5,DEFAULT_ANGLE_4};

//ANTICIPATION
//interest sequences
static const int interest_seq1[]={40};
static const int interest_seq2[]={DEFAULT_ANGLE_2-2};
static const int interest_seq3[]={DEFAULT_ANGLE_3};
static const int interest_seq4[]={DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4-30,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4-30,DEFAULT_ANGLE_4+10,};
//anticipation sequences
static const int anticipation_seq1[]={40};
static const int anticipation_seq2[]={DEFAULT_ANGLE_2-2};
static const int anticipation_seq3[]={DEFAULT_ANGLE_3};
static const int anticipation_seq4[]={DEFAULT_ANGLE_4,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4-30,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4-30,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4-30,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4+10,DEFAULT_ANGLE_4-30,DEFAULT_ANGLE_4+10,};
//vigilance sequences
static const int vigilance_seq1[]={40,40,40,40,40,40,40,40,40,40,30,40,30,40,30,40};
static const int vigilance_seq2[]={DEFAULT_ANGLE_2-2};//
static const int vigilance_seq3[]={DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3-10,DEFAULT_ANGLE_3,DEFAULT_ANGLE_3+10,DEFAULT_ANGLE_3};//
static const int vigilance_seq4[]={DEFAULT_ANGLE_4+10};//

//DISGUST
//boredom sequences
static const int boredom_seq1[]={100};
static const int boredom_seq2[]={DEFAULT_ANGLE_2-4,DEFAULT_ANGLE_2-4,DEFAULT_ANGLE_2-4,DEFAULT_ANGLE_2-4,DEFAULT_ANGLE_2-4};
static const int boredom_seq3[]={DEFAULT_ANGLE_3};
static const int boredom_seq4[]={0,45,90,120,120,120,120};
//disgust sequences
static const int disgust_seq1[]={20};
static const int disgust_seq2[]={DEFAULT_ANGLE_2-10};
static const int disgust_seq3[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const int disgust_seq4[]={5};
//loathing sequence
static const int loathing_seq1[]={70,50,70,50,70,50,70,50,70,50,70,50,70,50};
static const int loathing_seq2[]={DEFAULT_ANGLE_2-8};
static const int loathing_seq3[]={20,20,20,20,20,20,20,30,40,30,20,30,40,30,20,30,40,30,20};
static const int loathing_seq4[]={5};

//FEAR
//apprehension sequences
static const int apprehension_seq1[] = {40,35,30,25};
static const int apprehension_seq2[] = {DEFAULT_ANGLE_2+2};          
static const int apprehension_seq3[] = {20,20,25,20,25,120,125,120,125,10,15,10,15,100,105,100,105,100,105};
static const int apprehension_seq4[] = {DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4, DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4-10,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,DEFAULT_ANGLE_4,};
//fear sequences
static const int fear_seq1[]={40,60,40,60,40,60,40,60,40,60};
static const int fear_seq2[]={DEFAULT_ANGLE_2-8};
static const int fear_seq3[]={20,100,20,100,30,90,30,90};
static const int fear_seq4[]={90,80,70};
//terror sequences
static const int terror_seq1[] = {0,0,80,75,80,75,80,75,80,75,80,75,80,75,80,75,80,75,80,75,80,75,80,75,80,75,80,75};
static const int terror_seq2[] = {DEFAULT_ANGLE_2-8,DEFAULT_ANGLE_2-8,DEFAULT_ANGLE_2-8,DEFAULT_ANGLE_2-8,DEFAULT_ANGLE_2-8,DEFAULT_ANGLE_2-6,DEFAULT_ANGLE_2,DEFAULT_ANGLE_2+4};
static const int terror_seq3[] = {90,90,90,90,90,5};
static const int terror_seq4[] = {DEFAULT_ANGLE_4, DEFAULT_ANGLE_4,10};

//JOY
//serenity sequences
static const int serenity_seq1[]={50,40,50,40,50};
static const int serenity_seq2[]={DEFAULT_ANGLE_2-6, DEFAULT_ANGLE_2+4,DEFAULT_ANGLE_2-6, DEFAULT_ANGLE_2+4,DEFAULT_ANGLE_2-6, DEFAULT_ANGLE_2+4};
static const int serenity_seq3[]={DEFAULT_ANGLE_3};
static const int serenity_seq4[]={DEFAULT_ANGLE_4};// add slow blink? like cats?
//joy sequences
static const int joy_seq1[]={50,40,50,40,50,40,50,40,50};
static const int joy_seq2[]={DEFAULT_ANGLE_2-4};
static const int joy_seq3[]={75, 100,75,100};
static const int joy_seq4[]={60};
//ecstasy sequences
static const int ecstasy_seq1[]={60,50,60,50,60,50,60,50,60,50,60};
static const int ecstasy_seq2[]={DEFAULT_ANGLE_2-2};
static const int ecstasy_seq3[]={DEFAULT_ANGLE_3};
static const int ecstasy_seq4[]={70};

//SADNESS
//pensiveness sequences
static const int pensiveness_seq1[]={70,70,60};
static const int pensiveness_seq2[]={ DEFAULT_ANGLE_2+1,DEFAULT_ANGLE_2-5,DEFAULT_ANGLE_2+10};
static const int pensiveness_seq3[]={DEFAULT_ANGLE_3-10};
static const int pensiveness_seq4[]={DEFAULT_ANGLE_4,40,40,40, 0};
//sadness sequences
static const int sadness_seq1[]={70,70,70,70,60};
static const int sadness_seq2[]={DEFAULT_ANGLE_2-4,DEFAULT_ANGLE_2+10};
static const int sadness_seq3[]={0,0,20,0,0,20,0};
static const int sadness_seq4[]={0};
//grief sequences
static const int grief_seq1[]={DEFAULT_ANGLE_1,DEFAULT_ANGLE_1,DEFAULT_ANGLE_1,70,70,70,70,70,70,70,65,70,70,70,70,70,65,70,70,70,70,70,65,70,70,70,70,65,70,65};
static const int grief_seq2[]={DEFAULT_ANGLE_2+12};
static const int grief_seq3[]={80};
static const int grief_seq4[]={0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,30,0,0,0,0,30,0,0,0,0,30,0};

//SURPRISE
//distraction sequences
static const int distraction_seq1[] = {15,0};
static const int distraction_seq2[] = {DEFAULT_ANGLE_2-8};
static const int distraction_seq3[] = {0, 0,0,0,0,0,0,0,0,0,0,0,180,180,180,180,180,180,180,180,180,180,90};
static const int distraction_seq4[] = {DEFAULT_ANGLE_4}; 
//surprise sequences
static const int surprise_seq1[] = {30};
static const int surprise_seq2[] = {DEFAULT_ANGLE_2-4, DEFAULT_ANGLE_2-8};
static const int surprise_seq3[] = {80,80,80,100,100,100,100,80,80,80,80,100,100,100,80,80,100};
static const int surprise_seq4[] = {DEFAULT_ANGLE_4, DEFAULT_ANGLE_4+10, DEFAULT_ANGLE_4+15};
//amazement sequences
static const int amazement_seq1[] = {70,70,70,70,70,65,68,65,68,65,68,65,68,65,68,65,68,65,68,65,68,65,68,65,68,65,70};
static const int amazement_seq2[] = {DEFAULT_ANGLE_2-8};
static const int amazement_seq3[] = {90};
static const int amazement_seq4[] = {DEFAULT_ANGLE_4, DEFAULT_ANGLE_4+10};

//TRUST
//acceptance sequences
static const int acceptance_seq1[]={};
static const int acceptance_seq2[]={};                         //?
static const int acceptance_seq3[]={};
static const int acceptance_seq4[]={};
//trust sequences
static const int trust_seq1[]={};
static const int trust_seq2[]={};                              //?
static const int trust_seq3[]={};
static const int trust_seq4[]={};
//admiration sequences
static const int admiration_seq1[]={90,90,90,90,90,90,90,90,90};
static const int admiration_seq2[]={DEFAULT_ANGLE_2+4,DEFAULT_ANGLE_2-10};
static const int admiration_seq3[]={DEFAULT_ANGLE_3};
static const int admiration_seq4[]={90};