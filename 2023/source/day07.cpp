#include "days.hpp"

#include "bblp/aoc/file_utils.hpp"
#include "bblp/aoc/point.hpp"
#include "bblp/aoc/string_utils.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bblp::aoc {
namespace {
enum class Type { HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, FULL_HOUSE, FOUR_OF_A_KIND, FIVE_OF_A_KIND };
enum class Card {
    CARD_2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_T,
    CARD_J,
    CARD_Q,
    CARD_K,
    CARD_A
};

enum class CardWithJoker {
    CARD_J,
    CARD_2,
    CARD_3,
    CARD_4,
    CARD_5,
    CARD_6,
    CARD_7,
    CARD_8,
    CARD_9,
    CARD_T,
    CARD_Q,
    CARD_K,
    CARD_A
};

Type calculateType(const std::vector<Card>& hand) {
    std::unordered_map<Card, uint32_t> cardsCount;
    for (const auto card : hand) {
        if (cardsCount.contains(card)) {
            cardsCount.at(card) += 1U;
        } else {
            cardsCount.insert({card, 1U});
        }
    }

    if (std::find_if(cardsCount.cbegin(), cardsCount.cend(), [](const auto& pair) { return pair.second == 5U; }) !=
        cardsCount.cend()) {
        return Type::FIVE_OF_A_KIND;
    } else if (std::find_if(cardsCount.cbegin(), cardsCount.cend(),
                            [](const auto& pair) { return pair.second == 4U; }) != cardsCount.cend()) {
        return Type::FOUR_OF_A_KIND;
    } else if (std::find_if(cardsCount.cbegin(), cardsCount.cend(),
                            [](const auto& pair) { return pair.second == 3U; }) != cardsCount.cend()) {
        if (std::find_if(cardsCount.cbegin(), cardsCount.cend(), [](const auto& pair) { return pair.second == 2U; }) !=
            cardsCount.cend()) {
            return Type::FULL_HOUSE;
        } else {
            return Type::THREE_OF_A_KIND;
        }
    } else if (auto iter = std::find_if(cardsCount.cbegin(), cardsCount.cend(),
                                        [](const auto& pair) { return pair.second == 2U; }) != cardsCount.cend()) {
        const auto num =
            std::accumulate(cardsCount.cbegin(), cardsCount.cend(), 0U, [](uint32_t sum, const auto& pair) {
                if (pair.second == 2U) {
                    return sum + 1U;
                } else {
                    return sum;
                }
            });
        if (num == 2U) {
            return Type::TWO_PAIR;
        } else {
            return Type::ONE_PAIR;
        }
    } else {
        return Type::HIGH_CARD;
    }
}

Type calculateTypeWithJoker(const std::vector<CardWithJoker>& hand) {
    std::unordered_map<CardWithJoker, uint32_t> cardsCount;
    for (const auto card : hand) {
        if (cardsCount.contains(card)) {
            cardsCount.at(card) += 1U;
        } else {
            cardsCount.insert({card, 1U});
        }
    }

    auto jokerIter = cardsCount.find(CardWithJoker::CARD_J);
    const auto jokerCount = jokerIter != cardsCount.cend() ? jokerIter->second : 0U;

    if (jokerCount == 5U || std::find_if(cardsCount.cbegin(), cardsCount.cend(),
                     [&jokerCount](const auto& pair) { return pair.second + jokerCount == 5U; }) !=
        cardsCount.cend()) {
        return Type::FIVE_OF_A_KIND;
    } else if (std::find_if(cardsCount.cbegin(), cardsCount.cend(),
                            [&jokerCount](const auto& pair) { return pair.first != CardWithJoker::CARD_J && pair.second + jokerCount == 4U; }) != cardsCount.cend()) {
        return Type::FOUR_OF_A_KIND;
    } else if (std::find_if(cardsCount.cbegin(), cardsCount.cend(), [&jokerCount](const auto& pair) {
                   return pair.second + jokerCount == 3U;
               }) != cardsCount.cend()) {
        auto iter = std::find_if(cardsCount.cbegin(), cardsCount.cend(),
                                 [&jokerCount](const auto& pair) { return pair.second + jokerCount == 3U; });

        if (std::find_if(cardsCount.cbegin(), cardsCount.cend(), [&iter](const auto& pair) {
                return pair.first != CardWithJoker::CARD_J && iter->first != pair.first && pair.second == 2U;
            }) !=
            cardsCount.cend()) {
            return Type::FULL_HOUSE;
        } else {
            return Type::THREE_OF_A_KIND;
        }
    } else if (auto iter = std::find_if(cardsCount.cbegin(), cardsCount.cend(), [&jokerCount](const auto& pair) {
                               return pair.second + jokerCount == 2U;
                           }) != cardsCount.cend()) {
        if (jokerCount > 0U) {
            return Type::ONE_PAIR;
        }

        const auto num =
            std::accumulate(cardsCount.cbegin(), cardsCount.cend(), 0U, [](uint32_t sum, const auto& pair) {
                if (pair.second == 2U) {
                    return sum + 1U;
                } else {
                    return sum;
                }
            });
        if (num == 2U) {
            return Type::TWO_PAIR;
        } else {
            return Type::ONE_PAIR;
        }
    } else {
        return Type::HIGH_CARD;
    }
}

std::vector<Card> convertToCards(const std::string& str) {
    std::vector<Card> cards(str.size());
    std::transform(str.cbegin(), str.cend(), cards.begin(), [](const char c) -> Card {
        switch (c) {
            case '2':
                return Card::CARD_2;
            case '3':
                return Card::CARD_3;
            case '4':
                return Card::CARD_4;
            case '5':
                return Card::CARD_5;
            case '6':
                return Card::CARD_6;
            case '7':
                return Card::CARD_7;
            case '8':
                return Card::CARD_8;
            case '9':
                return Card::CARD_9;
            case 'T':
                return Card::CARD_T;
            case 'J':
                return Card::CARD_J;
            case 'Q':
                return Card::CARD_Q;
            case 'K':
                return Card::CARD_K;
            case 'A':
                return Card::CARD_A;
            default:
                throw std::logic_error("Invalid card");
        }
    });
    return cards;
}

std::vector<CardWithJoker> convertToCardsWithJoker(const std::string& str) {
    std::vector<CardWithJoker> cards(str.size());
    std::transform(str.cbegin(), str.cend(), cards.begin(), [](const char c) -> CardWithJoker {
        switch (c) {
            case '2':
                return CardWithJoker::CARD_2;
            case '3':
                return CardWithJoker::CARD_3;
            case '4':
                return CardWithJoker::CARD_4;
            case '5':
                return CardWithJoker::CARD_5;
            case '6':
                return CardWithJoker::CARD_6;
            case '7':
                return CardWithJoker::CARD_7;
            case '8':
                return CardWithJoker::CARD_8;
            case '9':
                return CardWithJoker::CARD_9;
            case 'T':
                return CardWithJoker::CARD_T;
            case 'J':
                return CardWithJoker::CARD_J;
            case 'Q':
                return CardWithJoker::CARD_Q;
            case 'K':
                return CardWithJoker::CARD_K;
            case 'A':
                return CardWithJoker::CARD_A;
            default:
                throw std::logic_error("Invalid card");
        }
    });
    return cards;
}

struct Hand {
    Hand(const std::string& cards, uint64_t bid) : cards{convertToCards(cards)}, bid{bid}, type{calculateType(this->cards)} {}

    std::vector<Card> cards;
    uint64_t bid;
    Type type;

    std::weak_ordering operator<=>(const Hand& other) const;
};

struct HandWithJoker {
    HandWithJoker(const std::string& cards, uint64_t bid)
        : cards{convertToCardsWithJoker(cards)}, bid{bid}, type{calculateTypeWithJoker(this->cards)} {}

    std::vector<CardWithJoker> cards;
    uint64_t bid;
    Type type;

    std::weak_ordering operator<=>(const HandWithJoker& other) const;
};

std::weak_ordering Hand::operator<=>(const Hand& other) const {
    if (type > other.type) {
        return std::weak_ordering::greater;
    } else if (type < other.type) {
        return std::weak_ordering::less;
    } else {
        for (auto i = 0U; i < cards.size(); ++i) {
            if (cards.at(i) < other.cards.at(i)) {
                return std::weak_ordering::less;
            } else if (cards.at(i) > other.cards.at(i)) {
                return std::weak_ordering::greater;
            }
        }
        return std::weak_ordering::equivalent;
    }
}

std::weak_ordering HandWithJoker::operator<=>(const HandWithJoker& other) const {
    if (type > other.type) {
        return std::weak_ordering::greater;
    } else if (type < other.type) {
        return std::weak_ordering::less;
    } else {
        for (auto i = 0U; i < cards.size(); ++i) {
            if (cards.at(i) < other.cards.at(i)) {
                return std::weak_ordering::less;
            } else if (cards.at(i) > other.cards.at(i)) {
                return std::weak_ordering::greater;
            }
        }
        return std::weak_ordering::equivalent;
    }
}

auto parse(const std::filesystem::path& filePath) {
    static constexpr std::size_t BUFFER_SIZE = 10000;

    std::vector<std::pair<std::string, uint64_t>> input;
    const auto lineCallback = [&input](const std::string& line) {
        const auto parts = split(line, " ");
        input.emplace_back(parts.front(), std::stoul(parts.back()));
    };
    parseInput(filePath, lineCallback);
    return input;
}

std::ostream& operator<<(std::ostream& stream, CardWithJoker card) {
    switch (card) {
        case CardWithJoker::CARD_J:
            stream << "J";
            break;
        case CardWithJoker::CARD_2:
            stream << "2";
            break;
        case CardWithJoker::CARD_3:
            stream << "3";
            break;
        case CardWithJoker::CARD_4:
            stream << "4";
            break;
        case CardWithJoker::CARD_5:
            stream << "5";
            break;
        case CardWithJoker::CARD_6:
            stream << "6";
            break;
        case CardWithJoker::CARD_7:
            stream << "7";
            break;
        case CardWithJoker::CARD_8:
            stream << "8";
            break;
        case CardWithJoker::CARD_9:
            stream << "9";
            break;
        case CardWithJoker::CARD_T:
            stream << "T";
            break;
        case CardWithJoker::CARD_Q:
            stream << "Q";
            break;
        case CardWithJoker::CARD_K:
            stream << "K";
            break;
        case CardWithJoker::CARD_A:
            stream << "A";
            break;
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const std::vector<CardWithJoker>& cards) {
    for (auto card : cards) {
        stream << card;
    }

    return stream;
}

uint64_t calculatePartOne(const std::vector<std::pair<std::string, uint64_t>>& input) {
    uint64_t result{0U};
    std::vector<Hand> hands;
    hands.reserve(input.size());
    for (const auto& pair : input) {
        hands.emplace_back(pair.first, pair.second);
    }
    std::sort(hands.begin(), hands.end(), std::less<Hand>());
    for (auto i = 0U; i < hands.size(); ++i) {
        result += hands.at(i).bid * static_cast<uint64_t>(i + 1U);
    }
    return result;
}

uint64_t calculatePartTwo(const std::vector<std::pair<std::string, uint64_t>>& input) {
    uint64_t result{0U};
    std::vector<HandWithJoker> hands;
    hands.reserve(input.size());
    for (const auto& pair : input) {
        hands.emplace_back(pair.first, pair.second);
    }
    std::sort(hands.begin(), hands.end(), std::less<HandWithJoker>());
    for (auto i = 0U; i < hands.size(); ++i) {
        result += hands.at(i).bid * static_cast<uint64_t>(i + 1U);
    }
    return result;
}
}  // namespace

std::pair<std::string, std::string> day07() {
    const auto input = parse("resources/day07.txt");

    return {std::to_string(calculatePartOne(input)), std::to_string(calculatePartTwo(input))};
}
}  // namespace bblp::aoc
