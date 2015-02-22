// Copyright (C) 2013 Matej Groma; licensed under GPL 3 (see LICENSE)
#pragma once
#define rules_provider rules_config

auto rules_flip=[](size_t ar,bool prev) -> bool
{
	return prev^true;
};
auto rules_random=[](size_t ar,bool prev) -> bool
{
	return (rand() % 2) != 0;
};
auto rules_bui=[](size_t ar,bool prev) -> bool
{
	if ((ar == 2) || (ar == 3) || (ar == 5))return false;
	return true;
};
auto rules_conway=[](size_t ar,bool prev) -> bool
{
	if (!prev)return ar == 3;
	if ((ar >= 2) && (ar <= 3))return true;
	return false;
};
auto rules_config=[](size_t ar,bool prev) -> bool
{
	int* rules=(prev ? conf->rules_for_alive : conf->rules_for_dead);
	int w=rules[ar];
	if (w == 3)return prev;
	return ((w == 2) ? rand_bool(conf->rules_rand_threshold) : (w != 0));
};