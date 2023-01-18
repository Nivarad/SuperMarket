#define CHECK_RETURN_0(ptr, result){\
    if (ptr == NULL)\
        result = 0;\
    else\
        result = 1;\
}
#define CHECK_MESSAGE_RETURN_0(ptr,res,msg){\
	if (ptr == NULL){\
		printf("%s",msg);\
		res=0;\
	}\
	else\
		res=1;\
}
#define FREE_CLOSE_FILE_RETURN_0(fp,res,obj){\
	free(obj);\
	fclose(fp);\
	res=0;\
}
#define CLOSE_RETURN_0(fp,res){\
	fclose(fp);\
	res= 0;\
}
