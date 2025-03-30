#include <algorithm>
#include <array>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace medale
{
#define LICZBA_MEDALI 3

using medals_count = std::array<uint32_t, LICZBA_MEDALI>;
using new_classification = std::array<uint32_t, 3>;
using medal_ranking = std::unordered_map<std::string, medals_count>;
using medal_weight = std::uint32_t;
using medal_count = std::uint32_t;
using medal_score = std::uint64_t;
using kraj_wynik = std::pair<std::string, uint64_t>;
using country_scores = std::vector<kraj_wynik>;
using input_size = std::uint32_t;

const std::string country_name_pattern = "([A-Z][a-zA-Z ]*[a-zA-Z])";
const std::string country_medal_won_pattern =
    country_name_pattern + " (0|1|2|3)";
const std::string country_medal_taken_pattern =
    "-" + country_name_pattern + " (1|2|3)";
const std::string weights_pattern = "([1-9][0-9]{0,5})";
const std::string weights_input_pattern =
    "=" + weights_pattern + " " + weights_pattern + " " + weights_pattern;

enum MedalType
{
    noMedal = 0,
    Gold = 1,
    Silver = 2,
    Bronze = 3
};
uint32_t medal_to_index(MedalType medal) { return medal - 1; }

void print_error(uint32_t line_num)
{
    std::cerr << "ERROR " << line_num << std::endl;
}

void handle_medal_won_input(const std::smatch& match, medal_ranking& ranking)
{
    std::string country = match[1].str();
    MedalType medal = static_cast<MedalType>(std::stoi(match[2].str()));

    // if country exists in a map we will not insert anything
    auto [it, inserted] = ranking.emplace(country, new_classification{0, 0, 0});
    if (medal != noMedal)
    {
        it->second[medal_to_index(medal)]++;
    }
}

void handle_medal_taken_input(const std::smatch& match, medal_ranking& ranking,
                              const input_size line_count)
{
    std::string country = match[1].str();
    MedalType medal = static_cast<MedalType>(std::stoi(match[2].str()));
    auto it = ranking.find(country);

    // we will not get out of range since regex ensures that medal equals one of
    // 1/2/3.
    if (it == ranking.end() || it->second[medal_to_index(medal)] <= 0)
        print_error(line_count);
    else
    {
        it->second[medal_to_index(medal)]--;
    }
}

void handle_print_classification_input(const std::smatch& match,
                                       const medal_ranking& ranking)
{
    if (ranking.empty()) return;
    medal_weight gold_weight = std::stoi(match[1].str());
    medal_weight silver_weight = std::stoi(match[2].str());
    medal_weight bronze_weight = std::stoi(match[3].str());
    country_scores scores;

    for (const auto& entry : ranking)
    {
        const std::string& country = entry.first;
        const medals_count& medalCount = entry.second;

        medal_count gold_count = medalCount[medal_to_index(Gold)];
        medal_count silver_count = medalCount[medal_to_index(Silver)];
        medal_count bronze_count = medalCount[medal_to_index(Bronze)];
        medal_score score = (gold_count * gold_weight) +
                            (silver_count * silver_weight) +
                            (bronze_count * bronze_weight);
        scores.push_back({country, score});
    }
    std::sort(scores.begin(), scores.end(),
              [](const kraj_wynik& a, const kraj_wynik& b)
              {
                  if (a.second == b.second)
                  {
                      return a.first < b.first;
                  }
                  return a.second > b.second;
              });

    uint32_t current_rank = 1;
    for (size_t i = 0; i < scores.size(); ++i)
    {
        const auto& [country, score] = scores[i];
        std::cout << (i == 0 || score != scores[i - 1].second ? current_rank = i + 1
                                                    : current_rank)
                  << ". " << country << std::endl;
    }
}

int medal_classification()
{
    std::string input;
    std::smatch match;
    const std::regex medal_won_regex(country_medal_won_pattern);
    const std::regex medal_taken_regex(country_medal_taken_pattern);
    const std::regex print_classification_regex(weights_input_pattern);
    input_size line_count = 1;
    medal_ranking ranking;

    while (getline(std::cin, input))
    {
        if (std::regex_match(input, match, medal_won_regex))
            handle_medal_won_input(match, ranking);
        else if (std::regex_match(input, match, medal_taken_regex))
            handle_medal_taken_input(match, ranking, line_count);
        else if (std::regex_match(input, match, print_classification_regex))
            handle_print_classification_input(match, ranking);
        else
            print_error(line_count);
        line_count++;
    }
    return 0;
}
}  

int main() { return medale::medal_classification(); }
