#include <chrono>
#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <vector>

using namespace std;

queue<char> split_string_to_queue(string base) {
    queue<char> char_list;
    for (unsigned i = 0; i < base.length(); i++)
        char_list.push(base[i]);
    return char_list;
}

vector<char> split_string_to_vector(string base) {
    vector<char> char_list;
    for (unsigned i = 0; i < base.length(); i++)
        char_list.push_back(base[i]);
    return char_list;
}

string stack_flip_to_string(stack<char> st) {
    string buffer = "";
    while (not st.empty()) {
        buffer += st.top();
        st.pop();
    }

    string result = "";
    for (int i = (int) buffer.length() - 1; i >= 0; i--)
        result += buffer[i];

    return result;
}

vector<char> generate_dictionary(vector<char> base_queue, char excluded_char) {
    vector<char> dictionary;
    bool single_match = false;
    for (unsigned i = 0; i < base_queue.size(); i++) {
        if (base_queue[i] != excluded_char || single_match) {
            dictionary.push_back(base_queue[i]);
        }
        else
            single_match = true;
    }
    return dictionary;
}

queue<char> dictionary_to_choice_queue(vector<char> base_queue, char excluded_char) {
    queue<char> choice_queue;
    bool single_match = false;
    for (unsigned i = 0; i < base_queue.size(); i++) {
        if (base_queue[i] != excluded_char || single_match) {
            choice_queue.push(base_queue[i]);
        }
        else
            single_match = true;
    }
    return choice_queue;
}

vector<string> generate_permutate(string base) {
    // FIXME : While unique char is working properly, repetition will cause multiple print
    vector<string> permutations;

    stack<queue<char>> choice_stack;
    stack<vector<char>> dictionary_stack;

    queue<char> base_queue = split_string_to_queue(base);
    choice_stack.push(base_queue);
    vector<char> base_dict = split_string_to_vector(base);
    dictionary_stack.push(base_dict);
    // Initial choices
    string generated_string = "";
    stack<char> used_char;
    while (not choice_stack.empty()) {
        queue<char>& available_choice = choice_stack.top();

        if (not available_choice.empty()) {
            used_char.push(available_choice.front());
            vector<char> leftover_char = generate_dictionary(dictionary_stack.top(), available_choice.front());
            queue<char> leftover_queue = dictionary_to_choice_queue(leftover_char, available_choice.front());
            dictionary_stack.push(leftover_char);
            available_choice.pop();
            choice_stack.push(leftover_queue);
        }
        else {
            choice_stack.pop();
            dictionary_stack.pop();
            if (used_char.size() == base.length()) {
                generated_string = stack_flip_to_string(used_char);
                permutations.push_back(generated_string);
            }
            if (not used_char.empty())
                used_char.pop();
        }
    }

    return permutations;
}

int main() {
    string base;
    cout << "Input = ";
    cin >> base;

    vector<string> permute = generate_permutate(base);

    for (unsigned i = 0; i < permute.size(); i++)
        cout << permute[i] << endl;

    cout << "Total = " << permute.size() << endl;
    return 0;
}
