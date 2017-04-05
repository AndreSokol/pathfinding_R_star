#ifndef OPENCONTAINER_H
#define OPENCONTAINER_H

#include <set>

template <typename T>
class OpenContainer
{
private:
    bool _gmax;
    bool _removeDuplicate = true;

    struct _Cmp {
        bool _i_gmax;

        // FIXME
        void Cmp(bool new_gmax){
            _i_gmax = new_gmax;
        }

        bool operator() (const T &a, const T &b) const {
            if      (a.AVOID && !b.AVOID) return false;
            else if (!a.AVOID && b.AVOID) return true;

            if      (a.F < b.F) return true;
            else if (a.F > b.F) return false;

            if(_i_gmax) {
                if      (a.g < b.g) return true;
                else if (a.g > b.g) return false;
            } else {
                if      (a.g > b.g) return true;
                else if (a.g < b.g) return false;
            }

            if      (a.i < b.i) return true;
            else if (a.i > b.i) return false;

            return a.j < b.j;
        }
    };

    std::set<T, _Cmp> _data;

public:
    OpenContainer(std::string breaking_ties) {
        if(breaking_ties == "g-max") _gmax = true;
        else                        _gmax = false;
    }

    OpenContainer(std::string breaking_ties, bool removeDuplicate) {
        if(breaking_ties == "g-max") _gmax = true;
        else                        _gmax = false;

        _removeDuplicate = removeDuplicate;
    }

    bool find(const T &el) {
        return (_data.count(el) != 0);
    }

    void push(T el) {
        if (!_removeDuplicate) {
            _data.insert(el);
            return;
        }

        auto it = _data.find(el);
        if (it == _data.end()) _data.insert(el);
        else {
            if (it->g > el.g) {
                it->g = el.g;
                it->H = el.H;
                it->F = el.F;
                it->parent = el.parent;
            }
        }
    }

    T pop() {
        auto max_element_it = _data.begin();

        T max_element = *max_element_it;
        _data.erase(max_element_it);

        return max_element;
    }

    bool empty() {
        return _data.empty();
    }

    unsigned int size() {
        return _data.size();
    }
};

#endif // OPENCONTAINER_H
