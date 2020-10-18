add_contract_with_checked_abi(commun.bonus commun.bonus commun.bonus.abi src/commun.bonus.cpp)
install_contract(commun.bonus)
target_include_directories(commun.bonus.wasm
   PUBLIC
   ${TOKEN_INCLUDE_PATH}
   ${CMAKE_CURRENT_SOURCE_DIR}/include
   ${CMAKE_CURRENT_SOURCE_DIR}/../cyberway.contracts/cyber.token/include
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_SOURCE_DIR}/../include)
