library ieee;
use ieee.std_logic_1164.all;

entity test is

end entity;

architecture beh of test is
	
signal clk : std_logic;

signal a : std_logic;

begin
ad : process(clk) is
begin
if rising_edge(clk) then
a <= '1';
end if;
end process;
end architecture;
