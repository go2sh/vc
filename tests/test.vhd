library ieee;
use ieee.std_logic__1164.all;
use ieee.std__1164.all;

entity test is

end entity;

architecture beh of test is

signal a : integer := 5 + 3 mod 4;

signal clk : std_logic;

signal a : std_logic;

subtype ar is natural range 0 to 20;
type ina is array(natural range <>) of ar;
type ina2 is array(0 to 10) of integer;
signal asd : ina2;

function "and"(a,b: integer) return integer is
begin
  return 0;
end function;

begin
ad : process(clk) is
begin
if rising_edge(clk) then
a <= '1' and '1' or '1';
asd <= (others => 0);
end if;
end process;
end architecture;
