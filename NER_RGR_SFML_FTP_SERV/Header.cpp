#include "Header.h";





int convertTo_Int(string str) {
	int answer = 0;
	int multiplay = 10;
	if (str.size())
		answer = str[0] - '0';
	for (int i = 1; i < str.size(); i++) {
		answer *= 10;
		answer += str[i] - '0';
	}
	return answer;
}



std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}


string PATH_EXE_PROGRAMM() {
	setlocale(LC_ALL, "");
	WCHAR buffer[MAX_PATH];

	GetModuleFileName(NULL, buffer, sizeof(buffer) / sizeof(buffer[0]));

	//string path = (char*)buffer;
	wstring path_w = buffer;
	string path(path_w.begin(), path_w.end());
	for (auto it = path.end(); it != path.begin(); --it) {
		if (it != path.end()) {
			if (*it == '\\') {
				break;
				path.erase(it);
			}
			else {
				path.erase(it);
			}
		}
	}
	return path;
}

inline bool PATHIsDirectory(const std::string& path) {
	bool answer = false;
	struct stat s;
	if (stat(path.c_str(), &s) == 0) {
		if (s.st_mode & S_IFDIR)
		{
			answer = true;
		}
	}
	//return GetFileAttributes(path.c_str()) & (FILE_ATTRIBUTE_DIRECTORY == FILE_ATTRIBUTE_DIRECTORY);
	return answer;
}


list<PATH> LIST_S(string relative_path) {
	list<PATH> answer;
	if (relative_path.size()) {
		if (*relative_path.begin() == '/' || *relative_path.begin() == '\\')
			relative_path.erase(relative_path.begin());


		for (auto it = relative_path.begin(); it != relative_path.end(); it++) {
			if (*it == '/')
				(*it) = '\\';
		}
	}

	string slash;
	if (relative_path.size() && *(--relative_path.end()) != '\\')
		slash = '\\';


	string path = PATH_EXE_PROGRAMM();
	WIN32_FIND_DATAW wfd;
	cout << path + relative_path + slash + '*' << endl;
	std::wstring stemp = s2ws(path + relative_path + slash + '*');
	LPCWSTR path2_0 = stemp.c_str();

	HANDLE const hFind = FindFirstFileW(path2_0, &wfd);

	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			wstring str = &wfd.cFileName[0];
			string line_(str.begin(), str.end());
			PATH line; line.full_path = path + relative_path + slash + line_;
			line.path = line_;
			//  /*
			if (PATHIsDirectory(line.full_path)) {
				line.time = data_izmemeniya_f(line.full_path.c_str(), false);
				line.isDirectory = true;
				line.linux_rights = "drwxrwxrwx";
			}
			else {
				line.time = data_izmemeniya_f(line.full_path.c_str());
				line.linux_rights = "-rwxrwxrwx";
				line.isDirectory = false;
				line.sizeB = getFileSize(line.full_path.c_str());
			}


			if (line.path != "." && line.path != "..")
				answer.push_back(line);
			//std::wcout << &wfd.cFileName[0] << std::endl;
		} while (NULL != FindNextFileW(hFind, &wfd));

		FindClose(hFind);
	}
	return answer;
}

int getFileSize(const char* fileName) {
	struct stat file_stat;
	stat(fileName, &file_stat);
	return file_stat.st_size;
}


void ParsMessage(char* buffer, int sizeBuffer, vector<string>& msg_parts) {

	int count_pars_word = 0;
	for (int i = 0; buffer[i] != 0 && i < sizeBuffer; i++) {
		if (!msg_parts.size()) {
			if (buffer[i] == ' ')		continue;
			else {
				msg_parts.push_back(string()); count_pars_word++;
			}
		}
		if (buffer[i] != ' ') {
			msg_parts.back() += buffer[i];
		}
		else {
			msg_parts.push_back(string());
			count_pars_word++;
		}
	}
}


string cutData(string msg, bool isFile) {
	string msg_pars[5];
	int count_pars_word = 0;
	bool space = false;

	for (auto it = msg.begin(); it != msg.end(); it++) {
		if ((*it) == ' ') {
			if (space) {
				msg.erase(it);
				space = false;
				continue;
			}
			space = true;
		}
		else
			space = false;
	}

	for (int i = 0; msg[i] != 0 && i < sizeof(msg); i++) {
		if (!count_pars_word) {
			if (msg[i] == ' ')		continue;
			else			count_pars_word++;
		}
		if (msg[i] != ' ')
			msg_pars[count_pars_word - 1] += msg[i];
		else
			count_pars_word++;
	}
	string ans;
	if (isFile) {
		msg_pars[2].erase(--msg_pars[2].end()); msg_pars[2].erase(--msg_pars[2].end()); msg_pars[2].erase(--msg_pars[2].end());
		ans = msg_pars[0] + ' ' + msg_pars[1] + ' ' + msg_pars[2];
	}
	else {
		ans = msg_pars[0] + ' ' + msg_pars[1] + ' ' + msg_pars[3];
	}
	return ans;
}

string data_izmemeniya_f(const char* file, bool isFile)
{
	struct _stat st;
	_stat(file, &st);
	char mod[100];
	ctime_s(mod, 100, &st.st_mtime);
	string answ = mod;
	answ.erase(--answ.end());
	answ.erase(answ.begin()); answ.erase(answ.begin()); answ.erase(answ.begin()); answ.erase(answ.begin());

	answ = (isFile) ? cutData(answ) : cutData(answ, false);

	return answ;
}




void ConvertToUnixPATH(string& path) {
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == '\\')
			path[i] = '/';
	}
}

void ConvertToWindowsPath(string& path) {
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == '/')
			path[i] = '\\';
	}
}

bool isRootDirectory(string path) {
	bool ans = false;
	ConvertToUnixPATH(path);
	if (path.size() == 1 && path[0] == '/')
		ans = true;
	return ans;
}


void deleteEndSlash(string& str) {

}

void deleteBeginSlash(string& str) {
	ConvertToUnixPATH(str);
	if (str.size()) {
		if (*str.begin() == '/')
			str.erase(str.begin());
	}
}

void addEndSlach(string& str) {
	ConvertToUnixPATH(str);
	if (str.size()) {
		if (*--str.end() != '/')
			str += '/';
	}
}

string getPreviousPath(string str) {
	ConvertToUnixPATH(str);
	string tmp = str;
	string previous;
	addBeginSlash(tmp);
	deleteBeginSlash(tmp);

	if (str.size()) {
		addBeginSlash(tmp);
		if (!isRootDirectory(tmp)) {
			int countSlach = 0;
			for (auto it = tmp.begin(); it != tmp.end(); it++) {
				if (*it == '/')
					countSlach++;
			}
			if(countSlach == 1)
				return string("/");

			previous = '/';
			bool flag = false;
			for (auto it = tmp.begin(); it != tmp.end(); it++) {
				if (*it == '/' && countSlach)
					countSlach--;
				if (!countSlach)
					previous += *it;
			}
		}
		else
			previous = '/';
	}
	else
		previous = '/';
	return previous;
}

void addBeginSlash(string& str) {
	ConvertToUnixPATH(str);
	if (str.size()) {
		if (*str.begin() != '/')
			str = '/' + str;
	}
	else
		str = '/';
}


