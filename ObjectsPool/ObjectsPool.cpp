// This sample illustrates the best way to implement
// object pooling inside a context class, when pooled objects 
// couldn't be maken default constructible and depend on 
// a context object member.
// (C) Andrey Minenkov, 2018.

#include <memory>
#include <vector>
#include <locale>
#include <sstream>
#include <algorithm>
#include <iostream>


class Context {
public:
  class Entity {
  public:
    Entity( std::locale & locale ) { m_formatter.imbue( locale ); }
    std::wstringstream m_formatter;
  };

  class EntityPool {
  public:
    typedef std::vector< std::unique_ptr< Entity > > Pool;

    EntityPool( std::locale & locale ) : m_locale( locale ) { } 
  
    const Pool & Get() const { return m_pool; } 
    void Resize( std::size_t size );
  private:
    Pool m_pool;
    std::locale & m_locale;
  };

public:
  Context( std::locale & locale ) : m_pool( locale ) { }

  std::wstring Do();

private:
  EntityPool m_pool;
};


void Context::EntityPool::Resize( std::size_t size )
{
  const std::size_t prevSize = m_pool.size();
  m_pool.resize( size );

  for( unsigned i = prevSize; i < size; ++i )
    m_pool[ i ].reset( new Entity( m_locale ) );
}

std::wstring Context::Do()
{
  const auto & pool = m_pool.Get();
  m_pool.Resize( 1 );
  pool.back()->m_formatter << L"Some front text" << std::endl;
  m_pool.Resize( 0x10 );
  pool.back()->m_formatter << L"Some back text" << std::endl;
  const std::wstring result = pool.front()->m_formatter.str() + m_pool.Get().back()->m_formatter.str();
  // Can not make inconsistent - compile error:
  //pool.back().reset();
  return result;
}


int main()
{
  std::locale locale( "" );
  Context context( locale );
  std::wcout << context.Do();

  return 0;
}