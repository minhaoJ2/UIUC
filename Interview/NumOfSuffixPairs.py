class TrieNode:
    def __init__(self):
        self.children = {}

class Trie:
    def __init__(self):
        self.root = TrieNode()
        
    def insert(self, word):
        node = self.root
        for i in range(len(word) - 1, -1, -1):
            if word[i] not in node.children:
                node.children[word[i]] = TrieNode()
            node = node.children[word[i]]
    
    def endsWith(self, suffix):
        node = self.root
        for i in range(len(suffix) - 1, -1, -1):
            if suffix[i] not in node.children:
                return False
            node = node.children[suffix[i]]
        return self.countNumber(node)
    
    def countNumber(self, node):
        count = 0
        if len(node.children) == 0:
            return count
        if len(node.children) != 0:
            count += 1
        for i in node.children:
            count += self.countNumber(node.children[i])
        return count
    
def countNumberOfPairs(words):
    trie = Trie()
    for word in words:
        trie.insert(word)
    count= 0
    for i in range(len(words)):
        print(trie.endsWith(words[i]))
        count += trie.endsWith(words[i])
    return count
