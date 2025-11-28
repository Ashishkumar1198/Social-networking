#include <bits/stdc++.h>
using namespace std;

struct User {
    string name;
    int age = 0;
    string city;
};

struct Suggestion {
    string candidate;
    int mutuals;
    int distance; 
};

class SocialNetwork {
private:
   
    unordered_map<string, User> users;
    unordered_map<string, unordered_set<string>> adj; 

public:
  
    bool hasUser(const string& name) const {
        return users.find(name) != users.end();
    }

    bool addUser(const string& name, int age = 0, const string& city = "") {
        if (hasUser(name)) return false;
        users[name] = User{name, age, city};
        adj[name]; 
        return true;
    }


    bool deleteUser(const string& name) {
        if (!hasUser(name)) return false;

    
        for (auto& kv : adj) {
            kv.second.erase(name);
        }
        adj.erase(name);
        users.erase(name);
        return true;
    }


    bool addFriendship(const string& a, const string& b) {
        if (a == b) return false;
        if (!hasUser(a) || !hasUser(b)) return false;
        bool existed = (adj[a].count(b) > 0);
        adj[a].insert(b);
        adj[b].insert(a);
        return !existed;
    }

    bool deleteFriendship(const string& a, const string& b) {
        if (!hasUser(a) || !hasUser(b)) return false;
        if (a == b) return false;

        bool existed = (adj[a].count(b) > 0);
        adj[a].erase(b);
        adj[b].erase(a);
        return existed;
    }

    

    vector<string> getFriends(const string& name) const {
        vector<string> out;
        auto it = adj.find(name);
        if (it == adj.end()) return out;
        out.reserve(it->second.size());
        for (auto &f : it->second) out.push_back(f);
        sort(out.begin(), out.end());
        return out;
    }

    void printFriends(const string& name) const {
        if (!hasUser(name)) { cout << "User not found.\n"; return; }
        auto friends = getFriends(name);
        cout << name << " has " << friends.size() << " friend(s): ";
        for (size_t i = 0; i < friends.size(); ++i) {
            cout << friends[i];
            if (i + 1 != friends.size()) cout << ", ";
        }
        cout << "\n";
    }



    vector<Suggestion> suggestByMutuals(const string& src, int topK = 5) const {
        vector<Suggestion> res;
        if (!hasUser(src)) return res;
        const auto& srcFriends = adj.at(src);
        unordered_map<string, int> mutualCount; 
        for (const string& f : srcFriends) {
            auto it = adj.find(f);
            if (it == adj.end()) continue;
            for (const string& fof : it->second) {
                if (fof == src) continue;
                if (srcFriends.count(fof)) continue;
                mutualCount[fof]++;
            }
        }        
        unordered_map<string, int> dist;
        limitedBFS(src, 3, dist);
        struct Node { int mutuals; int distance; string name; };
        auto cmp = [](const Node& a, const Node& b){
            if (a.mutuals != b.mutuals) return a.mutuals < b.mutuals;
            if (a.distance != b.distance) return a.distance > b.distance;
            return a.name > b.name; 
        };
        priority_queue<Node, vector<Node>, decltype(cmp)> pq(cmp);
        for (auto &kv : mutualCount) {
            string cand = kv.first;
            int m = kv.second;
            int d = dist.count(cand) ? dist[cand] : INT_MAX; 
            pq.push(Node{m, d, cand});
        }
        while (!pq.empty() && (int)res.size() < topK) {
            auto n = pq.top(); pq.pop();
            res.push_back(Suggestion{n.name, n.mutuals, n.distance});
        }
        return res;
    }

 
    vector<string> shortestPath(const string& src, const string& dst) const {
        vector<string> empty;
        if (!hasUser(src) || !hasUser(dst)) return empty;
        if (src == dst) return {src};

        unordered_map<string, string> parent; 
        queue<string> q;
        unordered_set<string> vis;
        q.push(src); vis.insert(src);

        bool found = false;
        while (!q.empty() && !found) {
            string u = q.front(); q.pop();
            auto it = adj.find(u);
            if (it == adj.end()) continue;
            for (const string& v : it->second) {
                if (vis.count(v)) continue;
                vis.insert(v);
                parent[v] = u;
                if (v == dst) { found = true; break; }
                q.push(v);
            }
        }
        if (!found) return empty;
    
        vector<string> path;
        for (string cur = dst; cur != ""; cur = (parent.count(cur) ? parent[cur] : "")) {
            path.push_back(cur);
            if (cur == src) break;
        }
        reverse(path.begin(), path.end());
        return path;
    }

   
    vector<pair<string,int>> topPopularUsers(int k = 5) const {
        struct Node { int deg; string name; };
        auto cmp = [](const Node& a, const Node& b){
            if (a.deg != b.deg) return a.deg < b.deg; 
            return a.name > b.name; 
        };
        priority_queue<Node, vector<Node>, decltype(cmp)> pq(cmp);

        for (auto &kv : adj) {
            pq.push(Node{(int)kv.second.size(), kv.first});
        }
        vector<pair<string,int>> out;
        while (!pq.empty() && (int)out.size() < k) {
            auto n = pq.top(); pq.pop();
            out.push_back({n.name, n.deg});
        }
        return out;
    }

   
    void seedSample() {
        vector<tuple<string,int,string>> sample = {
            {"Ashish", 20, "Phagwara"}, {"Rahul", 21, "Delhi"}, {"Priya", 20, "Mumbai"},
            {"Neha", 22, "Noida"}, {"Arjun", 23, "Lucknow"}, {"Simran", 21, "Chandigarh"},
            {"Karan", 22, "Jaipur"}, {"Meera", 19, "Pune"}, {"Dev", 24, "Indore"},
            {"Anya", 20, "Kolkata"}
        };
        for (auto &t : sample) addUser(get<0>(t), get<1>(t), get<2>(t));

  
        addFriendship("Ashish", "Rahul");
        addFriendship("Ashish", "Arjun");
        addFriendship("Rahul", "Priya");
        addFriendship("Priya", "Neha");
        addFriendship("Arjun", "Karan");
        addFriendship("Karan", "Simran");
        addFriendship("Simran", "Priya");
        addFriendship("Meera", "Dev");
        addFriendship("Dev", "Anya");
        addFriendship("Neha", "Karan");
        addFriendship("Rahul", "Karan");
    }


    void limitedBFS(const string& src, int maxDepth, unordered_map<string,int>& dist) const {
        queue<string> q;
        unordered_set<string> vis;
        q.push(src); vis.insert(src);
        dist[src] = 0;
        while (!q.empty()) {
            string u = q.front(); q.pop();
            int du = dist[u];
            if (du == maxDepth) continue;
            auto it = adj.find(u);
            if (it == adj.end()) continue;
            for (const string& v : it->second) {
                if (vis.count(v)) continue;
                vis.insert(v);
                dist[v] = du + 1;
                q.push(v);
            }
        }
    }

   
    void printGraph() const {
        cout << "\n--- Graph (Adjacency Lists) ---\n";
        for (auto &kv : adj) {
            cout << kv.first << ": ";
            vector<string> tmp(kv.second.begin(), kv.second.end());
            sort(tmp.begin(), tmp.end());
            for (size_t i = 0; i < tmp.size(); ++i) {
                cout << tmp[i];
                if (i + 1 != tmp.size()) cout << ", ";
            }
            cout << "\n";
        }
        cout << "------------------------------\n";
    }
};

static string readLine(const string& prompt = "") {
    if (!prompt.empty()) {
        cout << prompt << flush;
    }
    string s;
    getline(cin >> ws, s);
    return s;
}

static int readInt(const string& prompt = "") {
    if (!prompt.empty()) {
        cout << prompt << flush; 
    }
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid number. Try again: " << flush;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return x;
}


static void pressEnterToContinue() {
    cout << "\nPress ENTER to continue..."; cin.ignore(numeric_limits<streamsize>::max(), '\n');
}


void printMenu() {
    cout << "\n===========================================\n";
    cout << " Social Network Friend Suggestion System\n";
    cout << "===========================================\n";
    cout << "1) Add User\n";
    cout << "2) Add Friendship (Undirected)\n";
    cout << "3) Show Friends of a User\n";
    cout << "4) Suggest Friends (by Mutuals)\n";
    cout << "5) Shortest Connection Path (BFS)\n";
    cout << "6) Top Popular Users (by Degree)\n";
    cout << "7) Seed Sample Data\n";
    cout << "8) Print Graph (Adjacency Lists)\n";
    cout << "9) Delete User\n";
    cout << "10) Delete Friendship\n";

    cout << "0) Exit\n"<< flush;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    SocialNetwork sn;

    while (true) {
        printMenu();
        int choice = readInt("\nEnter choice: ");
        if (choice == 0) {
            cout << "Goodbye!\n";
            break;
        }
        switch (choice) {
            case 1: {
                string name = readLine("Enter name: ");
                int age = readInt("Enter age (0 if unknown): ");
                string city = readLine("Enter city (optional): ");
                bool ok = sn.addUser(name, age, city);
                cout << (ok ? "User added.\n" : "User already exists.\n");
                break;
            }
            case 2: {
                string a = readLine("User A: ");
                string b = readLine("User B: ");
                if (!sn.hasUser(a) || !sn.hasUser(b)) {
                    cout << "Both users must exist. Use option 1 to add users.\n";
                } else {
                    bool created = sn.addFriendship(a, b);
                    if (created) cout << "Friendship added.\n";
                    else cout << "They were already friends.\n";
                }
                break;
            }
            case 3: {
                string name = readLine("Enter user name: ");
                sn.printFriends(name);
                break;
            }
            case 4: {
                string name = readLine("Suggest friends for (user): ");
                int k = readInt("How many suggestions (K): ");
                auto s = sn.suggestByMutuals(name, k);
                if (s.empty()) { cout << "No suggestions available (add more friendships / users).\n"; break; }
                cout << "\nSuggestions for " << name << ":\n";
                for (size_t i = 0; i < s.size(); ++i) {
                    cout << i+1 << ") " << s[i].candidate
                         << "  | mutual friends: " << s[i].mutuals;
                    if (s[i].distance != INT_MAX) cout << "  | distance: " << s[i].distance;
                    cout << "\n";
                }
                break;
            }
            case 5: {
                string a = readLine("Source user: ");
                string b = readLine("Target user: ");
                auto path = sn.shortestPath(a, b);
                if (path.empty()) cout << "No connection path found (check users / friendships).\n";
                else {
                    cout << "Shortest path (" << path.size()-1 << " hop(s)): ";
                    for (size_t i = 0; i < path.size(); ++i) {
                        cout << path[i];
                        if (i + 1 != path.size()) cout << " -> ";
                    }
                    cout << "\n";
                }
                break;
            }
            case 6: {
                int k = readInt("Top K popular users: ");
                auto top = sn.topPopularUsers(k);
                if (top.empty()) { cout << "No users yet.\n"; break; }
                cout << "\nTop popular users by degree:\n";
                for (size_t i = 0; i < top.size(); ++i) {
                    cout << i+1 << ") " << top[i].first << "  (friends: " << top[i].second << ")\n";
                }
                break;
            }
            case 7: {
                sn.seedSample();
                cout << "Sample users and friendships added.\n";
                break;
            }
            case 8: {
                sn.printGraph();
                break;
            }
            case 9: {
                string name = readLine("Enter user name to delete: ");
                bool ok = sn.deleteUser(name);
                cout << (ok ? "User deleted.\n" : "User not found.\n");
                break;
            }
            case 10: {
                string a = readLine("User A: ");
                string b = readLine("User B: ");
                bool ok = sn.deleteFriendship(a, b);
                cout << (ok ? "Friendship deleted.\n" : "No such friendship.\n");
                break;
            }
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
