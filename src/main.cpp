#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std;

// History System
vector<string> history;
const int MAX_HISTORY = 100;

void addToHistory(const string& cmd) {
	if (history.size() >= MAX_HISTORY)
		history.erase(history.begin());
	history.push_back(cmd);
}

// Token Parsing
vector<string> parse(const string& input) {
	vector<string> tokens;
	stringstream ss(input);
	string word;
	while (ss >> word) tokens.push_back(word);
	return tokens;
}

bool hasPipe(const vector<string>& tokens) {
	for (const string& t : tokens) if (t == "|") return true;
	return false;
}

pair<vector<string>, vector<string>> splitAtPipe(const vector<string>& tokens) {
	vector<string> left, right;
	bool seenPipe = false;
	for (const string& t : tokens) {
		if (t == "|") { seenPipe = true; continue; }
		if (seenPipe) right.push_back(t);
		else left.push_back(t);
	}
	return {left, right};
}

// Built-in Shell Commands
bool handleBuiltin(const vector<string>& tokens) {
	if (tokens.empty()) return true;
	string cmd = tokens[0];

	if (cmd == "exit") { cout << "Goodbye!\n"; exit(0); }

	if (cmd == "cd") {
		if (tokens.size() < 2) {
			if (chdir(getenv("HOME")) != 0) perror("myshell: cd home");
		} else {
			if (chdir(tokens[1].c_str()) != 0) perror("myshell: cd");
		}
		return true;
	}
	if (cmd == "pwd") {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd))) cout << cwd << "\n";
		return true;
	}
	if (cmd == "history") {
		for (int i = 0; i < (int)history.size(); i++)
			cout << "  " << i + 1 << "  " << history[i] << "\n";
		return true;
	}
	if (cmd == "help") {
		cout << "\nmyshell -- A custom Unix shell built in C++\n\n";
		cout << "Built-in commands:\n";
		cout << "  cd [dir]   change directory\n";
		cout << "  pwd        print working directory\n";
		cout << "  history    show command history\n";
		cout << "  exit       quit the shell\n";
		cout << "  help       show this menu\n\n";
		cout << "Features: piping (|), redirection (> <), background (&)\n\n";
		return true;
	}
	return false;
}

// Standard Command Execution (with Redirection & Background support)
void execute(vector<string> tokens) {
	if (tokens.empty()) return;

	bool runInBackground = false;
	if (tokens.back() == "&") {
		runInBackground = true;
		tokens.pop_back();
	}

	string outputFile = "", inputFile = "";
	vector<string> cleanTokens;
	for (int i = 0; i < (int)tokens.size(); i++) {
		if (tokens[i] == ">" && i + 1 < (int)tokens.size()) outputFile = tokens[++i];
		else if (tokens[i] == "<" && i + 1 < (int)tokens.size()) inputFile = tokens[++i];
		else cleanTokens.push_back(tokens[i]);
	}

	if (cleanTokens.empty()) return;

	vector<char*> args;
	for (string& s : cleanTokens) args.push_back(&s[0]);
	args.push_back(nullptr);

	pid_t pid = fork();
	if (pid == 0) {
		if (!outputFile.empty()) {
			int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
			dup2(fd, STDOUT_FILENO); close(fd);
		}
		if (!inputFile.empty()) {
			int fd = open(inputFile.c_str(), O_RDONLY);
			dup2(fd, STDIN_FILENO); close(fd);
		}
		execvp(args[0], args.data());
		cerr << "myshell: command not found: " << cleanTokens[0] << "\n";
		exit(1);
	} else if (pid > 0) {
		if (runInBackground) cout << "[" << pid << "]\n";
		else waitpid(pid, nullptr, 0);
	} else {
		cerr << "myshell: fork() failed\n";
	}
}

// Pipeline Execution Engine
void executePipe(vector<string>& left, vector<string>& right) {
	int fd[2]; 
	if (pipe(fd) == -1) {
		perror("myshell: pipe failed");
		return;
	}

	auto toArgs = [](vector<string>& t) {
		vector<char*> a;
		for (string& s : t) a.push_back(&s[0]);
		a.push_back(nullptr); return a;
	};
	auto la = toArgs(left), ra = toArgs(right);

	pid_t p1 = fork();
	if (p1 == 0) {
		dup2(fd[1], STDOUT_FILENO); close(fd[0]); close(fd[1]);
		execvp(la[0], la.data()); exit(1);
	}
	pid_t p2 = fork();
	if (p2 == 0) {
		dup2(fd[0], STDIN_FILENO); close(fd[1]); close(fd[0]);
		execvp(ra[0], ra.data()); exit(1);
	}
	close(fd[0]); close(fd[1]);
	waitpid(p1, nullptr, 0); waitpid(p2, nullptr, 0);
}

// REPL Main Loop
int main() {
	string input;
	cout << "Welcome to myshell. Type 'help' for commands.\n";

	while (true) {
		cout << "myshell> ";
		getline(cin, input);

		if (cin.eof()) { cout << "\nGoodbye!\n"; break; }
		if (input.empty()) continue;

		addToHistory(input);
		vector<string> tokens = parse(input);

		if (!handleBuiltin(tokens)) {
			if (hasPipe(tokens)) {
				auto [left, right] = splitAtPipe(tokens);
				
				// DEFENSIVE GUARD: Validate pipeline syntax boundaries
				if (left.empty() || right.empty()) {
					cerr << "myshell: syntax error near unexpected token '|'\n";
				} else {
					executePipe(left, right);
				}
			} else {
				execute(tokens);
			}
		}
	}
	return 0;
}
