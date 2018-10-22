#ifndef MUGRATREE_LIB
#define MUGRATREE_LIB

#include <cassert>
namespace mugratree
{
namespace detail
{
    template<typename G>
    class Type2Type
    {
        typedef G Type;
    };

    template<typename ...Args>
    class SmthInner;

    template<typename T, typename ...Args>
    class SmthInner<T, Args...>
    {
        union InnerUni
        {
            T m_head;
            SmthInner<Args...> m_tail;
        };

        InnerUni m_val;

        T GetImpl(Type2Type<T> nothing) const
        {
            return m_val.m_head;
        }

        template<typename G>
        G GetImpl(Type2Type<G> nothing) const
        {
            return m_val.m_tail.template Get<G>();
        }

        public:
            void Set(T t)
            {
                m_val.m_head = t;
            }

            template<typename G>
            void Set(G g)
            {
                m_val.m_tail.Set(g);
            }

            template<typename G>
            G Get() const
            {
                return GetImpl(Type2Type<G>());
            }

            void Dump() const
            {
                std::cout << m_val.m_head << std::endl;
                m_val.m_tail.Dump();
            }
    };

    template<>
    class SmthInner<>
    {
        public:
            void Dump() const {}
    };
} //detail

template<typename ...Ts>
class Smth
{
    detail::SmthInner<Ts...> m_inner;

    public:
        Smth() {}
        
        template<typename T>
        Smth(T t)
        {
            m_inner.Set(t);
        }

        template<typename T>
        void Set(T t)
        {
            m_inner.Set(t);
        }

        template<typename T>
        T Get() const
        {
            return m_inner.template Get<T>();
        }

        void Dump() const
        {
            m_inner.Dump();
        }
};

#if 0
template<typename ...Args>
class Tree
{
        Node* m_root;
    public:
        class Node
        {
            SmthInner<Args...> m_data;
            Node* m_lchild;
            Node* m_rchild;
            public:
                Node() : m_lchild(nullptr), m_rchild(nullptr) {}
                
                template<typename T>
                Node(T t) : m_data(t), m_lchild(nullptr), m_rchild(nullptr) {}
                Node(const Node& rhs) = delete;
                Node& operand=(const Node& rhs) = delete;

                template<typename T>
                void Set(T t)
                {
                    m_data.Set(t);
                }

                template<typename T>
                T Get() const
                {
                    return m_data.template Get<T>();
                }
                
                Node& GetLChild() const
                {
                    return *m_lchild;
                }

                Node& GetRChild() const
                {
                    return *m_rchild;
                }

                template<typename T>
                void InsertRChild(T t)
                {
                    m_rchild = new Node(t);
                }

        }
        const Node& GetRoot() const
        {
            return *m_root;
        }
}
#endif

} //mugratree
#endif //MUGRATREE_LIB
