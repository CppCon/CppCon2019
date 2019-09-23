#include <iostream>
#include <boost/asio/ts/executor.hpp>
#include <boost/asio/ts/internet.hpp>
#include <cppcon/accept/async_accept.hpp>

int main(int argc,
         char** argv)
{
  boost::asio::io_context ctx;
  boost::asio::ip::tcp::acceptor acc(ctx);
  boost::asio::ip::tcp::endpoint ep;
  ep.address(boost::asio::ip::address_v4());
  ep.port(11653);
  acc.open(ep.protocol());
  acc.bind(ep);
  acc.listen();
  auto ex = ctx.get_executor();
  cppcon::accept::async_accept(acc,
                               &ex,
                               &ex + 1,
                               [](auto) { std::cout << "Connection" << std::endl; },
                               [](auto ec) { std::cout << ec.message() << std::endl; });
  ctx.run();
}
