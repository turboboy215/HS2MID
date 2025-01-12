# HS2MID
Hudson Soft (GB/GBC) to MIDI converter

This tool converts music from Game Boy and Game Boy Color games using Hudson Soft's sound engine to MIDI format.

It works with ROM images. To use it, you must specify the name of the ROM followed by the number of the bank containing the sound data (in hex).
For games that contain multiple banks of music, you must run the program multiple times specifying where each different bank is located. However, in order to prevent files from being overwritten, the MIDI files from the previous bank must either be moved to a separate folder or renamed. In many games, though, the song numbers between different banks do not "overlap" (the numbers are just null pointers for other banks), so it is safe to overwrite files in those cases. Note also that the first track of most, if not all games is empty. This is normal.
For Pokémon Card GB2, Game Boy Wars 3, and Poyon no Dungeon Room 2, these are unusual that every bank stores the exact same pointer table, but the song data is only present in one of the banks. This will result in garbled/incoherent MIDIs for songs from the "wrong" bank, so you have to toggle between the different banks until you find the correct one.
This tool was based on disassemblies of Pokémon Trading Card Game and its Japan-only sequel, which can be found at the following links:
https://github.com/pret/poketcg
https://github.com/pret/poketcg2

Supported games:
  * Bakuten Shoot Beyblade
  * Bakutsu Retsouden Shou Hyper Fishing
  * Beyblade: Tournament Fighting
  * Bomberman Collection
  * Bomberman GB
  * Bomberman GB 3
  * Bomberman Max: Blue Champion
  * Bomberman Max: Red Challenger
  * Bonk's Revenge/B.C. Kid 2
  * Captain Tsubasa
  * Cardcaptor Sakura: Itsumo Sakura-chan to Issho
  * Daikaijuu Monogatari: The Miracle of the Zone II
  * Dodge Danpei
  * Dynablaster
  * Eurosport Pro Champ Fishing
  * Felix the Cat
  * From TV Animation Slam Dunk: Gakeppuchi no Kesshou League
  * From TV Animation Slam Dunk 2: Zenkoku heno Tip Off
  * Game Boy Wars 2
  * Game Boy Wars 3
  * Game Conveni 21
  * GB Genjin Land: Viva Chikkun Oukoku
  * Get Mushi Club: Minna no Konchu Daizukan
  * Guruguru Garakutas
  * Hamster Paradise
  * Hana Yori Dango: Another Love Story
  * Hexcite: The Shapes of Victory
  * Jikuu Senki Mu
  * Jisedai Begoma Battle Beyblade
  * Kanji de Puzzle
  * Karamucho Ziken
  * Karamuchou wa Oosawagi!: Okawari!
  * Karamuchou wa Oosawagi!: Porinkiis to Okashina Nakamatachi
  * Kaseki Sousei Reborn
  * Kaseki Sousei Reborn II: Monster Digger
  * Kawaii Pet Shop Monogatari 2
  * Keibajou he Ikou! Wide
  * Konchuu Hakase 2
  * Love Hina Party
  * Love Hina Pocket
  * Mario Golf
  * Mogu Mogu Gombo
  * Mogu Mogu Q
  * Momotarou Collection
  * Momotarou Collection 2
  * Momotarou Densetsu Gaiden
  * Nakayoshi Cooking Series 1: Oishii Cake Okusan
  * Nakayoshi Cooking Series 2: Oishii Panya-San
  * Nectaris GB
  * Pachi Pachi Pachi-Slot: New Pulsar Hen
  * Pachinko CR Mouretsu Genshijin T
  * Pocket Bomberman
  * Pocket Family GB2
  * Pocket no Naka no Oukoku (prototype)
  * Pokémon Card GB2: GR Dan Sanjou!
  * Pokémon Trading Card Game
  * Poyon no Dungeon Room
  * Poyon no Dungeon Room 2
  * Purikura Pocket: Fukanzen Joshikousei Manual
  * Purikura Pocket 2: Kareshi Kaizou Daisakusen
  * Robopon: Star Version
  * Robopon: Sun Version
  * Robot Pokottsu: Moon Versions
  * Sakura Taisen GB2: Thunder Volt Sakusen
  * Same Game
  * Shin Megami Tensei Devil Children: Aka no Shou
  * Shin Megami Tensei Devil Children: Kuro no Shou
  * Shin Megami Tensei Devil Children: Shiro no Shou
  * SS Spinner: Yo for It
  * Super B-Daman: Fighting Phoenix
  * Super Black Bass
  * Super Black Bass: Real Fight
  * Super Black Bass Pocket 2
  * Super Momotarou Densetsu
  * Super Robot Taisen: Link Battler
  * Wario Blast Featuring Bomberman!
  * Watashi no Kitchen
  * Watashi no Resutoran
  * Wizardry Empire
  * Wizardry Empire I: Fukkatsu no Tsue
  * Z: The Miracle of the Zone

## To do:
  * Panning support
  * GBS file support
