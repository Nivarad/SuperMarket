#pragma once

#include <stdio.h>
#include "Supermarket.h"

int		saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
			const char* customersFileName);
int		loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
			const char* customersFileName);

//int		loadCustomerFromTextFile(SuperMarket* pMarket, const char* customersFileName);


int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName);
int		loadSuperMarketFromCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName);
Product* loadProductFromCompressedTextFile(SuperMarket* pMarket, FILE* fp);
int		loadCompanyNameFromCompressedFile(SuperMarket* pMarket, FILE* fp, BYTE data);
int		loadComapnyLocationFromCompressedFile(SuperMarket* pMarket, FILE* fp);
int saveSuperMarketIntoCompressedFile(SuperMarket* pMarket, const char* fileName, const char* customersFileName);
int saveSuperNameAndProductCountIntoCompressedFile(SuperMarket* pMarket, FILE* fp);
int saveProductsIntoCompressedFile(SuperMarket* pMarket, FILE* fp);
int saveProductToCompressedFile(Product* prod, FILE* fp);
int saveSuperAdressIntoCompressedFile(SuperMarket* pMarket, FILE* fp);
int saveCountAndPriceIntoCompressedFile(Product* prod, FILE* fp);

