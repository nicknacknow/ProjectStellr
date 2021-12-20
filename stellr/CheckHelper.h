#pragma once
#include "globals.h"

typedef bool(CheckFunc)(RBX::Instance player);

struct Check {
	Check(CheckFunc f, std::string n) : func(f), name(n) {}
	CheckFunc* func;
	std::string name;
	bool enabled = true;
};

std::vector<Check> Checks;

class CheckHelper {
public:
	void AddCheck(Check check) {
		Checks.emplace_back(check);
	}
	void AddCheck(std::string name, CheckFunc func) {
		Check check{ func, name };
		return AddCheck(check);
	}
	template <class lambda>
	void AddCheck(std::string name, lambda func) {
		Check check{ func, name };
		return AddCheck(check);
	}
	static bool DoChecks(RBX::Instance player) {
		for (Check check : Checks)
			if (check.enabled && !check.func(player)) return false;
		return true;
	}
	Check SearchCheck(std::string name) {
		for (Check check : Checks) if (!check.name.compare(name)) return check;
	}
};