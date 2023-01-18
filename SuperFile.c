#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include "myMacros.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	int res = 1;
	CHECK_MESSAGE_RETURN_0(fp, res, "Error open supermarket file to write\n");
	if (res == 0)
	{
		return 0;
	}
	if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
	{
		CLOSE_RETURN_0(fp, res);
		return res;
	}

	if (!saveAddressToFile(&pMarket->location, fp))
	{
		CLOSE_RETURN_0(fp, res);
		return res;
	}
	int count = getNumOfProductsInList(pMarket);

	if (!writeIntToFile(count, fp, "Error write product count\n"))
	{
		CLOSE_RETURN_0(fp, res);
		return res;
	}

	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (!saveProductToFile(pTemp, fp))
		{
			CLOSE_RETURN_0(fp, res);
			return res;
		}
		pN = pN->next;
	}

	CLOSE_RETURN_0(fp, res);
	if(res==1)
		return 0;

	saveCustomerToTextFile(pMarket->customerArr,pMarket->customerCount, customersFileName);

	return 1;
}
#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	int res = 1;
	CHECK_MESSAGE_RETURN_0(fp, res, "Error open company file\n");
	if (res==0)
	{
		return 0;
	}

	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		FREE_CLOSE_FILE_RETURN_0(fp, res, pMarket->name);
		return res;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		FREE_CLOSE_FILE_RETURN_0(fp, res, pMarket->name);
		return res;
	}

	int count;
	if (!readIntFromFile(&count, fp, "Error reading product count\n"))
	{
		FREE_CLOSE_FILE_RETURN_0(fp, res, pMarket->name);
		return res;
	}

	if (count > 0)
	{
		Product* pTemp;
		for (int i = 0; i < count; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(fp, res, pMarket->name);
				return res;
			}
			if (!loadProductFromFile(pTemp, fp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(fp, res, pMarket->name);
				return res;
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(fp, res, pMarket->name);
				return res;
			}
		}
	}


	res = 1;
	CLOSE_RETURN_0(fp, res);
	if (res == 1)
		return 0;
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName,&pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	int res = 1;
	CHECK_RETURN_0(fp, res, "Error open company file\n");
	if (res == 0)
	{
		return 0;
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		int res = 1;
		CLOSE_RETURN_0(fp, res);
		return res;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		free(pMarket->name);
		int res = 1;
		CLOSE_RETURN_0(fp, res);
		return res;
	}

	CLOSE_RETURN_0(fp, res);
	return res;

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	int count;
	char* garbage;
	int garbage1;
	garbage1=fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp = NULL;
	for (int i = 0; i < count; i++)
	{
		int res = 1;
		pTemp = (Product*)calloc(1, sizeof(Product));
		CHECK_RETURN_0(pTemp, res);
		if (res == 0)
			return 0;
		garbage=myGets(pTemp->name, sizeof(pTemp->name), fp);
		garbage=myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		garbage1=fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}
	int res = 1;
	CLOSE_RETURN_0(fp, res);
	return 1;
}
int loadSuperMarketFromCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{	
	
	FILE* fp = fopen(fileName, "rb");
	int res = 1;
	CHECK_MESSAGE_RETURN_0(fp, res, "Error open company file");
	if (res == 0)
	{
		return 0;
	}
	//Extract number of products
	BYTE data[2];
	if (fread(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;

	int productCount;
	productCount = ((data[0] << 2) | (data[1] >> 6));
	loadCompanyNameFromCompressedFile(pMarket, fp, data[1]);

	loadComapnyLocationFromCompressedFile(pMarket, fp);

	L_init(&pMarket->productList);
	for (int i = 0; i < productCount; i++) {
		Product* pProd = loadProductFromCompressedTextFile(pMarket, fp);
		L_insert_end(&pMarket->productList.head, pProd);
	}
	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return 1;

}


Product* loadProductFromCompressedTextFile(SuperMarket* pMarket, FILE* fp)
{
	Product* pProd = (Product*)malloc(sizeof(Product));
	int res = 1;
	CHECK_RETURN_0(pProd, res);
	if (res == 0)
		return 0;
	//extract product Barcode
	BYTE prudctBuffer1[3];
	BYTE prudctBuffer2[3];
	fread(&prudctBuffer1, 1, 3, fp);

	fread(&prudctBuffer2, 1, 3, fp);
	if (prudctBuffer1 == NULL || prudctBuffer2 == NULL)
		return 0;
	char barcode[8] = { 0 };
	barcode[0] = (prudctBuffer1[0] & 0b11111100) >> 2;
	barcode[1] = (prudctBuffer1[0] & 0x3) << 4 | ((prudctBuffer1[1] & 0b11110000) >> 4);
	barcode[2] = ((prudctBuffer1[1] & 0b00001111) <<2)| ((prudctBuffer1[2] &0b11000000) >> 6);
	barcode[3] = prudctBuffer1[2] & 0x3f;
	barcode[4] = prudctBuffer2[0] >> 2;
	barcode[5] = (prudctBuffer2[0] & 0x3) << 4 | prudctBuffer2[1] >> 4;
	barcode[6] = ((prudctBuffer2[1] & 0b00001111) << 2) | ((prudctBuffer2[2]&0b11000000) >> 6);
	barcode[BARCODE_LENGTH] = '\0';

	for (int i = 0; i < BARCODE_LENGTH; i++) {
		decipherBarcode(&barcode[i]);
	}


	memcpy(pProd->barcode, barcode, 8);

	//extract product nameLength

	int nameLength = (prudctBuffer2[2] & 0x3c) >> 2;

	//extract product type
	int productType = prudctBuffer2[2] & 0x3;
	pProd->type = productType;

	//extract product name
	char* productName = NULL;
	productName = (char*)malloc(sizeof(char) * nameLength + 1);
	if (!productName)
		return 0;

	if (fread(productName, sizeof(char), nameLength, fp) != nameLength)
		return 0;
	productName[nameLength] = '\0';
	memcpy(pProd->name, productName, nameLength + 1);

	BYTE buff;
	//extract count of product

	if (fread(&buff, 1, 1, fp) != 1)
		return 0;


	pProd->count = buff;

	//extract price
	BYTE price[2];
	fread(&price, 1, 2, fp);
	int agorot = (price[0] & 0b11111110) >> 1;
	int fullNumPrice = (price[0] & 0x1) << 8 | price[1];
	pProd->price = (float)fullNumPrice + ((float)agorot / 100);
	return pProd;
}




int loadCompanyNameFromCompressedFile(SuperMarket* pMarket, FILE* fp, BYTE data)
{

	//extract company name length
	int nameLength = data & 0b00111111;

	char* name = (char*)calloc(1, nameLength + 1);
	if (!name)
		return 0;
	name[nameLength] = '\0';

	//extract company name
	if (fread(name, sizeof(BYTE), nameLength, fp) != nameLength)
		return 0;


	pMarket->name = name;
	return 1;
}

int loadComapnyLocationFromCompressedFile(SuperMarket* pMarket, FILE* fp)
{
	BYTE buff;
	if (fread(&buff, 1, 1, fp) != 1)
		return 0;

	pMarket->location.num = buff;

	pMarket->location.street = readStringFromFileWithout0(fp, "error read street name\n");
	if (!pMarket->location.street)
		return 0;

	//pMarket->location.street = (char*)realloc(pMarket->location.street, strlen(pMarket->location.street)+1);
	pMarket->location.street[strlen(pMarket->location.street)] = '\0';


	pMarket->location.city = readStringFromFileWithout0(fp, "error read street name\n");
	if (!pMarket->location.city)
		return 0;
	return 1;
	
}
int saveSuperMarketIntoCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName)
{
	FILE* fp = fopen(fileName, "wb");
	if (!saveSuperNameAndProductCountIntoCompressedFile(pMarket, fp))
		return 0;
	if (!saveSuperAdressIntoCompressedFile(pMarket, fp))
		return 0;
	if (!saveProductsIntoCompressedFile(pMarket, fp))
		return 0;
	return 1;
}
int saveProductsIntoCompressedFile(SuperMarket* pMarket, FILE* fp)
{
	int productCount = L_size(&pMarket->productList), i;
	NODE* head = pMarket->productList.head.next;
	for (i = 0; i < productCount; i++)
	{

		//Product* pProd = (Product*)pMarket->productList.head.next->key;
		if (!saveProductToCompressedFile((Product*)head->key, fp))
			return 0;
		if (!saveCountAndPriceIntoCompressedFile((Product*)head->key, fp))
			return 0;
		head = head->next;
	}
	return 1;
}
int saveProductToCompressedFile(Product* prod, FILE* fp)
{
	BYTE data[6] = { 0 };
	int nameLen = (int)strlen(prod->name);
	// A 0b0100000100  
	for (int i = 0; i < BARCODE_LENGTH; i++) {
		cipherBarcode(&prod->barcode[i]);
	}//00001011
	data[0] = ((prod->barcode[0] &0b00111111) << 2) | ((prod->barcode[1]& 0b00110000) >> 4);
	data[1] = ((prod->barcode[1]) & 0b00001111) << 4 | ((prod->barcode[2] & 0b00111100) >> 2);
	data[2] = ((prod->barcode[2] & 0b11) << 6) | (prod->barcode[3] &0b00111111);
	data[3] = (prod->barcode[4]) << 2 | (prod->barcode[5] & 0b11000000) >> 6;
	data[4] = ((prod->barcode[5] & 0b00001111) << 4) | ((prod->barcode[6] & 0b00111100) >>2 );
	data[5] = (prod->barcode[6] & 0x3) << 6 | (nameLen & 0b00001111) << 2 | prod->type;
	if (fwrite(&data, sizeof(BYTE), 6, fp) != 6)
		return 0;
	if (!writeCharsToFile(prod->name, nameLen, fp, "Error writing super name to compressed file"))
		return 0;
	return 1;
}
int saveSuperAdressIntoCompressedFile(SuperMarket* pMarket, FILE* fp)
{
	BYTE data[1] = { 0 };
	int streetLen = (int)strlen(pMarket->location.street);
	int cityLen = (int)strlen(pMarket->location.city);
	data[0] = pMarket->location.num;

	if (fwrite(&data, sizeof(BYTE), 1, fp) != 1)
		return 0;
	if (!writeIntToFile(streetLen, fp, "Error writing street len to compressed file"))
		return 0;
	if (!writeCharsToFile(pMarket->location.street, streetLen, fp, "Error writing street to compressed file"))
		return 0;
	if (!writeIntToFile(cityLen, fp, "Error writing street len to compressed file"))
		return 0;
	if (!writeCharsToFile(pMarket->location.city, cityLen, fp, "Error writing city to compressed file"))
		return 0;
	return 1;
}
int saveCountAndPriceIntoCompressedFile(Product* prod, FILE* fp)
{
	BYTE data[3] = { 0 };
	int withoutAgorot = (int)(prod->price);
	// 69 0b01000101 --> 01000100 |1  -->01000101
	int agorot = (int) ((prod->price - withoutAgorot) * 100);

	data[0] = prod->count;
	data[1] = (agorot) << 1 | ((withoutAgorot) >> 8);
	data[2] = withoutAgorot;
	if (fwrite(&data, sizeof(BYTE), 3, fp) != 3)
		return 0;
	return 1;
}
int saveSuperNameAndProductCountIntoCompressedFile(SuperMarket* pMarket, FILE* fp)
{
	BYTE data[2] = { 0 };
	int productCount = L_size(&pMarket->productList);
	int len = (int)strlen(pMarket->name);
	data[0] = productCount >> 2;
	data[1] = (productCount & 0x3) << 6 | len;
	if (fwrite(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;
	if (!writeCharsToFile(pMarket->name, len, fp, "Error writing super name to compressed file"))
		return 0;
	return 1;
}

