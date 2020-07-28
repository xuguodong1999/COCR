#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <cassert>
#include <map>
#include <string>

namespace item {
    /** 代码出处： https://zhuanlan.zhihu.com/p/107062266
     */
    template<class ProductType_t>
    class IProductRegistrar {
    public:
        virtual ProductType_t *CreateProduct(const char *name) = 0;

    protected:
        IProductRegistrar() {
        }

        virtual ~IProductRegistrar() {
        }
    };

    template<class ProductType_t>
    class ProductFactory {
    public:
        static ProductFactory<ProductType_t> &Instance() {
            static ProductFactory<ProductType_t> instance;
            return instance;
        }

        void RegisterProdect(const char *className, IProductRegistrar<ProductType_t> *registrar) {
            m_ProductRegistry[className] = registrar;
        }

        ProductType_t *GetProduct(const char *className, const char *memberName = "") {
            auto it = m_ProductRegistry.find(className);
            if (it != m_ProductRegistry.end()) {
                return it->second->CreateProduct(memberName);
            }
            assert(false);
            return nullptr;
        }

    private:
        ProductFactory() {
        }

        ~ProductFactory() {
        }

        ProductFactory(const ProductFactory &);

        const ProductFactory &operator=(const ProductFactory &);

        std::map<std::string, IProductRegistrar<ProductType_t> *> m_ProductRegistry;
    };

    template<class ProductType_t, class ProductImpl_t>
    class ProductRegistrar : public IProductRegistrar<ProductType_t> {
    public:
        explicit ProductRegistrar(const char *className) {
            ProductFactory<ProductType_t>::Instance().RegisterProdect(className, this);
        }

        ProductType_t *CreateProduct(const char *memberName) {
            // !!! nullptr is here to satisify qgraphicsitem's constructor
            return new ProductImpl_t(nullptr, memberName);
        }

    private:
    };
} // namespace item
#endif // TEMPLATE_H
