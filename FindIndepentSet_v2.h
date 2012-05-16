// NO BARCODE FILTERING, NODES ARE SELECTED AT RANDOM TO ADD TO THE UNIQUE SET
class Network{
	bool *Used;
public:
	Network();
	vector<string> PutBarcodes; //Keep the putative barcodes
	unsigned long int NofPutBarcodes;
	vector <string> CommonSet;
	vector< vector <double> > Features;
	int GenerateRandomNode(void);
	void initialisevars(void);
	static int CalculateEditDistance(string&, string&);
	void RetrieveUniqueNodes(string);
	void PrintUniquePutBarcodes(const char*);
	static string RevComp(string);
};

Network::Network(){}

string Network::RevComp(string s){	
	int z;
	string s_revcom;
	for(z=(s.length()-1);z>=0;--z){
		switch (s[z]){
			case 'A' : { s_revcom.append("T"); break; }
			case 'C' : { s_revcom.append("G"); break; }
			case 'G' : { s_revcom.append("C"); break; }
			case 'T' : { s_revcom.append("A"); break; }
		}
	}

	return s_revcom;
}
void Network::initialisevars(void){
	int i;
	Used=(bool*) malloc((NofPutBarcodes)*sizeof(bool));
	for(i=0;i<NofPutBarcodes;i++)
		Used[i]=0;

//	EDDistribution=(unsigned long long int*) malloc(SeqLen*sizeof(unsigned long long int));
//	dd=(unsigned int*) malloc(NofPutBarcodes*sizeof(unsigned int));

}
int Network::CalculateEditDistance(string& s1, string& s2){
	
	int x,y;
	string s2_revcom;
	unsigned int d[SeqLen+1][SeqLen+1];
	const int len=SeqLen;

	d[0][0] = 0;
	
	for(x = 1; x <= len; ++x) {
		d[x][0] = x;
		d[0][x] = x;
	}

	for(x = 1; x <= len; ++x)
			for(y = 1; y <= len; ++y)
				d[x][y] = std::min( std::min(d[x - 1][y] + 1,d[x][y - 1] + 1),d[x - 1][y - 1] + (s1[x - 1] == s2[y - 1] ? 0 : 1) );
	
	if(d[len][len] > EditDistanceThreshold){
	  s2_revcom=Network::RevComp(s2);
		d[0][0] = 0;

		for(x = 1; x <= len; ++x) {
			d[x][0] = x;
			d[0][x] = x;
		}

		for(x = 1; x <= len; ++x)
			for(y = 1; y <= len; ++y)
				d[x][y] = std::min( std::min(d[x - 1][y] + 1,d[x][y - 1] + 1),d[x - 1][y - 1] + (s1[x - 1] == s2_revcom[y - 1] ? 0 : 1) );
	}

	return d[len][len];
}

int Network::GenerateRandomNode(void){

	unsigned long int rn;
	do{
		rn=NofPutBarcodes*rand()/(RAND_MAX+1.0); // Generate the first random node
	}while(Used[rn]);
	Used[rn]=1;
	return rn;
}
void Network::RetrieveUniqueNodes(string putbarcode){

long int k;
int i;
bool addtoset=1;
unsigned int editdist;


addtoset=1;
bool done=false;
#pragma omp parallel for default(shared) firstprivate(EditDistanceThreshold)
 for(k=0;k<CommonSet.size();++k){ // Make sure if the last selected node is not connected to already present nodes in the common set
   //#pragma omp critical
   if(!done){
     editdist=Network::CalculateEditDistance(putbarcode,CommonSet[k]); // Check reverse complement also
     if(editdist<=EditDistanceThreshold){
#pragma omp critical
       {
	 addtoset=0;
	 done=true;
       }
     }
   }
}

if(addtoset)
	CommonSet.push_back(putbarcode); //Add it to the set if it not connected to the previously added members
	
if(CommonSet.size()%500==0)
	cout << CommonSet.size() << "       Unique Barcodes Selected" << endl;

}

void Network::PrintUniquePutBarcodes(const char *fname){

	int i=0,j=0;
	ofstream outf(fname);
	
	for(i=0;i<CommonSet.size();i++){
		outf << CommonSet[i] << endl;
	}
	outf.close();

}