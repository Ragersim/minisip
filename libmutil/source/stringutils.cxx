/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


/* Name
 * 	split_in_lines.cxx
 * Author
 * 	Erik Eliasson, eliasson@it.kth.se, 2003
 * Purpose
 * 	Takes a string as argument and splits it at any new line.
*/

#include<config.h>

#include<libmutil/stringutils.h>
#include<libmutil/mtypes.h>
#include<string>
#include<iostream>

#ifdef _MSC_VER
static int nocaseequal(char c1, char c2){
	if ( ((c1>='A') && (c1<='Z')) ){
		return (c1==c2) || (c1 == (c2 - ('a'-'A')));
	}
	if ( (c1>='a') && (c1<='z') ){
		return (c1==c2) || (c1 == (c2 + ('a'-'A')));
	}
	return c1==c2;
}




//Fixes by Philippe Torrelli - Thanks
static int strncasecmp(const char *s1, const char *s2, int n){
	int i;
	for ( i=0; s1[i]!=0 && s2[i]!=0 && (n==-1 || (n!=-1 && i<n) ); i++){
		if ( !nocaseequal(s1[i],s2[i]) ){
			if (s1[i]<s2[i]){
				return -1;
			}else{
				return 1;
			}
		}
	}

	if(i != n && (s1[i] != s2[i] )) { 
		if (s1[i] == '\0' ){
			return -1;
		}else{
			return 1;
		}
	}
	return 0;
}


static int strcasecmp(const char *s1, const char *s2){
	return strncasecmp(s1,s2,-1);
}

#else
#include <strings.h>
#endif



using namespace std;


LIBMUTIL_API string quote(const string &str){
	if (str.length()>1 && str[0] == '"' && str[str.length() - 1] == '"')
		return str;
	else
		return "\"" + str + "\"";
}


LIBMUTIL_API string unquote(string str){
	if( str.length() > 1 && str[0] == '"' && str[str.length() - 1] == '"' )
		return str.substr(1, str.length() - 2);
	else
		return str;
}


LIBMUTIL_API std::vector<string> split(const string &s, bool do_trim, char delim, bool includeEmpty){
	std::vector<string> ret;
	
	if (s.size()==0)
		return ret;

	unsigned i=0;
	do{
		string line;
		while (!(i>(s.length()-1)) && s[i]!=delim)
			line+=s[i++];
		if (do_trim)
			line=trim(line);
		if (line.length()>0 || includeEmpty)
			ret.push_back(line);
		i++;
	}while (!(i>=s.length()));

	if ( s.size()>0 && s[s.length()-1]==delim && includeEmpty )
		ret.push_back("");
	
	return ret;
}

LIBMUTIL_API std::vector<string> splitLines(const string &s, bool do_trim){
	return split(s, do_trim, '\n',false);
}

LIBMUTIL_API bool isWS(char c){
	return (c==' ' || c=='\t' || c=='\n' || c=='\r');
}

LIBMUTIL_API string upCase(const string &s){
	size_t n=s.size();
	string ret(n,' ');
	string::const_iterator i=s.begin();
	string::iterator j=ret.begin();
	for ( ; i!=s.end(); i++, j++)
		*j = upCase(*i);
	return ret;
}

LIBMUTIL_API int upCase(char c){
	if ((c>='a') && (c<='z'))
		return c - ('a'-'A');
	else
		return c;
}

LIBMUTIL_API int strCaseCmp(const char *s1, const char* s2){
	return strcasecmp(s1,s2);
}

LIBMUTIL_API int strNCaseCmp(const char *s1, const char* s2, int n){
	return strncasecmp(s1,s2,n);
}

LIBMUTIL_API string trim(const string &line){
	size_t spacesFront = 0, spacesEnd = 0;
	int32_t idx;

	idx = 0;
	while( idx < (int)line.size() && isspace(line[idx]) ) {
		spacesFront++;
		idx++;
	}
	
	idx = (int)line.size() - 1 ;
	while( idx >= 0 && isspace(line[idx]) ) {
		spacesEnd++;
		idx--;
	}
	return line.substr( spacesFront, line.size() - ( spacesFront + spacesEnd ) );
}

LIBMUTIL_API string binToHex( const unsigned char * data, int length ){
	
	string result = "";
	char hex_number[3];
	
	for( int i = 0; i < length; i++ ){
		sprintf( hex_number, "%02x", *(data+i) );
		result += hex_number;
	}
	
	return result;
}


LIBMUTIL_API string itoa(int64_t i){
	char buf[30]; //should be enough for a 64 bit integer ...
	for (int j=0; j<16; j++)
		buf[j]=0;
	sprintf(buf,"%ld",i);
	return string(buf);
}

