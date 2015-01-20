/*
 * pool_wars.c
 *	Code used to generate the reports in "Pool Wars?" on hashingit.com
 *
 * Copyright (C) 2015 David Hudson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/*
 * calculate1()
 *	Generate simulation results for a "hash switching" attack.
 */
void calculate1(double pool1, double pool2, bool pool1_attack, bool pool2_attack, double dilution)
{
	char filename[128];
	sprintf(filename, "c1-p1-%.1f-%s-p2-%.1f-%s-d-%.1f.txt",
		pool1 * 100.0, pool1_attack ? "yes" : "no",
		pool2 * 100.0, pool2_attack ? "yes" : "no",
		dilution * 100.0);
	FILE *f = fopen(filename, "wb+");
	if (!f) {
		return;
	}

	/*
	 * How much of the network isn't under the control of our combattants?
	 */
	double others = 1.0 - pool1 - pool2;
	double lim = (pool1 > pool2) ? pool1 : pool2;
	double min_pool = (pool1 > pool2) ? pool2 : pool1;
	double step = 0.001;
	if (min_pool <= 0.1) {
		step = 0.0001;
		if (min_pool <= 0.01) {
			step = 0.00001;
		}
	}

	fprintf(f, "Start #1 (p1 attack p2: %s, p2 attack p1: %s), p1: %.2f, p2: %.2f, others: %.2f, dilution: %.2f, step: %f\n",
		pool1_attack ? "yes" : "no",
		pool2_attack ? "yes" : "no",
		pool1, pool2, others, dilution, step);

	for (double d = 0.0; d <= lim; d += step) {
		double d1 = pool1_attack ? ((d < pool1) ? d : pool1) : 0.0;
		double d2 = pool2_attack ? ((d < pool2) ? d : pool2) : 0.0;
		double total_cap = 1.0 - d1 - d2;

		double pool1_actual_cap = pool1 - d1;
		double pool2_actual_cap = pool2 - d2;

		double diluted_d1 = d1 * dilution;
		double diluted_d2 = d2 * dilution;

		double pool1_share_cap = pool1_actual_cap + diluted_d2;
		double pool2_share_cap = pool2_actual_cap + diluted_d1;

		double pool1_mined_reward = pool1_actual_cap / total_cap;
		double pool2_mined_reward = pool2_actual_cap / total_cap;

		double pool1_own_mined_reward = (pool1_share_cap > 0.0)
						? ((pool1_actual_cap / pool1_share_cap) * pool1_mined_reward)
						: 0.0;
		double pool2_own_mined_reward = (pool2_share_cap > 0.0)
						? ((pool2_actual_cap / pool2_share_cap) * pool2_mined_reward)
						: 0.0;

		double pool1_fake_mined_reward = (pool2_share_cap > 0.0)
						? ((diluted_d1 / (pool2_share_cap)) * pool2_mined_reward)
						: 0.0;
		double pool2_fake_mined_reward = (pool1_share_cap > 0.0)
						? ((diluted_d2 / (pool1_share_cap)) * pool1_mined_reward)
						: 0.0;

		double reward_pool1 = pool1_own_mined_reward + pool1_fake_mined_reward;
		double reward_pool2 = pool2_own_mined_reward + pool2_fake_mined_reward;
		double reward_others = (others / total_cap);
		fprintf(f, "%7.3f | %7.3f | %7.3f | %7.3f | %7.3f | %7.3f | %7.3f\n",
			100.0 * d,
			100.0 * reward_pool1, 100.0 * ((reward_pool1 / pool1) - 1.0),
			100.0 * reward_pool2, 100.0 * ((reward_pool2 / pool2) - 1.0),
			100.0 * reward_others, 100.0 * ((reward_others / others) - 1.0));
	}

	fclose(f);
}

/*
 * calculate2()
 *	Generate simulation results for a "new stealth hashing" attack.
 */
void calculate2(double pool1, double pool2, bool pool1_attack, bool pool2_attack, double dilution)
{
	char filename[128];
	sprintf(filename, "c2-p1-%.1f-%s-p2-%.1f-%s-d-%.1f.txt",
		pool1 * 100.0, pool1_attack ? "yes" : "no",
		pool2 * 100.0, pool2_attack ? "yes" : "no",
		dilution * 100.0);
	FILE *f = fopen(filename, "wb+");
	if (!f) {
		return;
	}

	/*
	 * How much of the network isn't under the control of our combattants?
	 */
	double others = 1.0 - pool1 - pool2;
	fprintf(f, "Start #2 (p1 attack p2: %s, p2 attack p1: %s), p1: %.2f, p2: %.2f, others: %.2f, dilution: %.2f\n",
		pool1_attack ? "yes" : "no",
		pool2_attack ? "yes" : "no",
		pool1, pool2, others, dilution);

	double lim = 0.2;
	double min_pool = (pool1 > pool2) ? pool2 : pool1;
	double step = 0.001;
	if (min_pool < 0.1) {
		step = 0.0001;
	}

	for (double d = 0.0; d <= lim; d += step) {
		double d1 = pool1_attack ? d : 0.0;
		double d2 = pool2_attack ? d : 0.0;
		double total_cap = 1.0;

		double pool1_actual_cap = pool1;
		double pool2_actual_cap = pool2;

		double diluted_d1 = d1 * dilution;
		double diluted_d2 = d2 * dilution;

		double pool1_share_cap = pool1_actual_cap + diluted_d2;
		double pool2_share_cap = pool2_actual_cap + diluted_d1;

		double pool1_mined_reward = pool1_actual_cap / total_cap;
		double pool2_mined_reward = pool2_actual_cap / total_cap;

		double pool1_own_mined_reward = (pool1_share_cap > 0.0)
						? ((pool1_actual_cap / pool1_share_cap) * pool1_mined_reward)
						: 0.0;
		double pool2_own_mined_reward = (pool2_share_cap > 0.0)
						? ((pool2_actual_cap / pool2_share_cap) * pool2_mined_reward)
						: 0.0;

		double pool1_fake_mined_reward = (pool2_share_cap > 0.0)
						? ((diluted_d1 / (pool2_share_cap)) * pool2_mined_reward)
						: 0.0;
		double pool2_fake_mined_reward = (pool1_share_cap > 0.0)
						? ((diluted_d2 / (pool1_share_cap)) * pool1_mined_reward)
						: 0.0;

		double reward_pool1 = pool1_own_mined_reward + pool1_fake_mined_reward;
		double reward_pool2 = pool2_own_mined_reward + pool2_fake_mined_reward;
		double reward_others = (others / total_cap);
		fprintf(f, "%7.3f | %7.3f | %7.3f | %7.3f | %7.3f | %7.3f | %7.3f\n",
			100.0 * d,
			100.0 * reward_pool1, 100.0 * ((reward_pool1 / ((pool1 + d1) / (1.0 + d1 + d2))) - 1.0),
			100.0 * reward_pool2, 100.0 * ((reward_pool2 / ((pool2 + d2) / (1.0 + d1 + d2))) - 1.0),
			100.0 * reward_others, 100.0 * ((reward_others / (others / (1.0 + d1 + d2))) - 1.0));
	}

	fclose(f);
}

int main(int argc, char **argv)
{
	/*
	 * Generate outputs for a series of different mining pool attack scenarios.
	 */
	calculate1(0.25, 0.25, true, false, 1.0);
	calculate1(0.25, 0.25, true, false, 0.75);
	calculate1(0.25, 0.25, false, true, 1.0);
	calculate1(0.25, 0.25, true, true, 1.0);
	calculate1(0.4, 0.4, true, false, 0.6);
	calculate2(0.25, 0.25, true, false, 1.0);
	calculate2(0.25, 0.25, true, false, 0.75);
	calculate2(0.25, 0.25, false, true, 1.0);
	calculate2(0.25, 0.25, true, true, 1.0);
	calculate1(0.1, 0.1, true, false, 1.0);
	calculate1(0.01, 0.01, true, false, 1.0);
	calculate1(0.1, 0.1, true, false, 0.9);
	calculate1(0.1, 0.1, false, true, 1.0);
	calculate1(0.1, 0.1, true, true, 1.0);
	calculate2(0.1, 0.1, true, false, 1.0);
	calculate2(0.1, 0.1, true, false, 0.9);
	calculate2(0.1, 0.1, false, true, 1.0);
	calculate2(0.1, 0.1, true, true, 1.0);
	calculate1(0.25, 0.01, true, false, 1.0);
	calculate1(0.25, 0.01, true, false, 0.8);
	calculate1(0.25, 0.01, false, true, 1.0);
	calculate1(0.25, 0.01, true, true, 1.0);
	calculate2(0.25, 0.01, true, false, 1.0);
	calculate2(0.25, 0.01, true, false, 0.8);
	calculate2(0.25, 0.01, false, true, 1.0);
	calculate2(0.25, 0.01, true, true, 1.0);
	calculate1(0.5, 0.01, true, false, 1.0);
	calculate1(0.5, 0.01, true, false, 0.52);
	calculate1(0.5, 0.01, false, true, 1.0);
	calculate1(0.5, 0.01, true, true, 1.0);
	calculate2(0.5, 0.01, true, false, 1.0);
	calculate2(0.5, 0.01, true, false, 0.52);
	calculate2(0.5, 0.01, false, true, 1.0);
	calculate2(0.5, 0.01, true, true, 1.0);
	calculate1(0.5, 0.05, true, false, 1.0);
	calculate1(0.5, 0.05, true, false, 0.5);
	calculate1(0.5, 0.05, false, true, 1.0);
	calculate1(0.5, 0.05, true, true, 1.0);
	calculate2(0.5, 0.05, true, false, 1.0);
	calculate2(0.5, 0.05, true, false, 0.5);
	calculate2(0.5, 0.05, false, true, 1.0);
	calculate2(0.5, 0.05, true, true, 1.0);
}

