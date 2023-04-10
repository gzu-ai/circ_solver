
#pragma once
#ifndef CICR_LF_GRAPHT_H
#define CICR_LF_GRAPHT_H
#include <unordered_set>
#include <unordered_map>
#include "CluaseSet.h"
namespace CicrLF {
    class Graph {

        template<class T>
        struct LoopInserter{
            T container;
            ClauseSet& nf;
            explicit LoopInserter(T _n,ClauseSet&_nf):container(_n),nf(_nf){}
            inline void operator()(unsigned int i){
                Atom* atom=  nf.getAtom((i>>1));
                if (atom->type==MINIMAL)
                *(container++)=atom;
            }
        };
        template<class T>
        struct VisitInserter{
            T container;
            explicit VisitInserter(T _n):container(_n){}
            inline void operator()(unsigned int i){
                *(container++)=i;
            }
        };
    public:
        Graph()= default;
        inline void reset(){
            _visitedPos=_loopVisitedList.rbegin();
        }
        bool getLoop(std::vector<Atom *> &loop,ClauseSet&set);

        void initGraph(ClauseSet &_clauseSet,const std::vector<Atom*>&);


    private:
        template<class T>
        LoopInserter<T> makeLoopInserter(T _n, ClauseSet&_nf){return LoopInserter<T>(_n, _nf);}
        template<class T>
        VisitInserter<T> makeVisitInserter(T _n){return VisitInserter<T>(_n);}
        std::vector<unsigned int> _loopVisitedList;
        std::vector<char> _nodeInfo;
        std::vector<char> _isNode;
        std::vector<unsigned int>::reverse_iterator _visitedPos;
        std::vector<std::vector<unsigned int>> _content;
        template<class T,class VisitedInfo,class _OutputIterator>
        void dfs(T &graph,VisitedInfo&, int node, _OutputIterator _result);
        template<class T,class VisitedInfo,class _OutputIterator>
        void loop_dfs(T &graph,VisitedInfo&,_OutputIterator list);
    };



    bool Graph::getLoop(std::vector<Atom *> &loop,ClauseSet&set ) {
        loop.clear();
        auto inserter= makeLoopInserter(std::back_inserter(loop), set);
        while (_visitedPos != _loopVisitedList.rend()) {
            int node = *(_visitedPos++);
            if (_nodeInfo[node])continue;
            dfs(_content, _nodeInfo, node, inserter);
            if (loop.empty())continue;
            inserter(node);
            std::sort(loop.begin(), loop.end());
            return true;
        }
        return !loop.empty();
    }

    template<class T, class VisitedInfo,class _OutputIterator>
    void Graph::dfs(T &graph,VisitedInfo& info, int node, _OutputIterator _result) {
        info[node]=true;
        auto it = graph[node];
        if (it.empty()) {
            return;
        }
        for (auto itt:it) {
            if (info[itt]) {
                continue;
            }
            dfs(graph, info,itt, _result);
            _result(itt);
        }
    }

    template<class T,class VisitedInfo,class _OutputIterator>
    void Graph::loop_dfs(T &graph,VisitedInfo &info, _OutputIterator _result) {
        unsigned int i= 0;
        for (auto &it:graph) {
            if (!info[i]) {
                dfs(graph,info, i, _result);
                _result(i);
            }
            ++i;
        }

    }

    void Graph::initGraph(ClauseSet&set,const std::vector<Atom *> &v) {
        _loopVisitedList.clear();
        _content.clear();
        int vars=set.vars();
        _loopVisitedList.reserve(vars * 2);
        _isNode.resize(vars*2, false);
        _nodeInfo.resize(vars * 2, false);
        std::vector<std::vector<int>> _reverse(vars * 2);
        _content.reserve(vars * 2);
        _content.resize(vars * 2, {});
        for (const auto &item : v){
            item->flag= true;
            _isNode[item->value<<1]=true;
        }
        for (const auto &item : set.getClauses()){
            std::vector<unsigned int> src;
            std::vector<unsigned int> dis;
            for (const auto &lit : *item->clause){
                Atom * atom=lit.value;
                if (atom->type==VARIABLE){
                    src.push_back((atom->value <<1) +lit.neg);
                    dis.push_back((atom->value <<1) +!lit.neg);
                    continue;
                }
                if (!atom->flag){
                    continue;
                }
                if (lit.neg){
                    dis.push_back((atom->value <<1));
                } else{
                    src.push_back((atom->value <<1));
                }

            }
            for (const auto &pos : src){
                std::copy_if(dis.begin(), dis.end(),std::back_inserter(_content[pos]),[&](unsigned int a){
                    return a>>1!= pos>>1;
                });
            }
            for (const auto &pos : dis){
                std::copy_if(src.begin(), src.end(),std::back_inserter(_reverse[pos]),[&](unsigned int a){
                    return a>>1!= pos>>1;
                });
            }
        }
        std::vector<char> visited;
        visited.resize(vars * 2, false);
        loop_dfs(_reverse, visited, makeVisitInserter(std::back_inserter(_loopVisitedList)));
        reset();
        for (const auto &item : v){
            item->flag= false;
        }
    }
}
#endif //CICR_LF_GRAPH_H
