// CHESS CHALLENGE 1V1
// Implementing the basic rules and functionalities before the real challenge begins ...
// Author: Marlow Nedelchev

#include <SFML/Graphics.hpp>
#include <vector>
#include <bitset>
#include <cstdint>
#include <iostream>

enum piece_type {PAWN, KNIGHT, BISHOP, ROOK, KING, QUEEN, NONE};
enum piece_color {WHITE, BLACK};

inline int get_LSB(uint64_t bitboard) {
    return __builtin_ctzll(bitboard);  // Count Trailing Zeros
}

struct position {
    unsigned int row, col : 3; //char is 1 byte size (values 0-7)
    // convert position to bit board function
    uint64_t bit_pos() {
        uint64_t bp = 0ULL;
        int shift = 8 * row + col + 1;
        bp |= (1ULL) << shift;
        return bp;
        // REWRITE THIS
    }
};

class Piece {
public:
    piece_type type;
    piece_color color;
    position pos;
    
    Piece(piece_type t, piece_color c, position p) : type(t), color(c), pos(p) {}
    
    // add virtual get possible moves function
    virtual uint64_t get_moves(uint64_t white_pieces, uint64_t black_pieces) const = 0;
    virtual ~Piece() = default;
};

class Pawn : public Piece {
public:
    bool has_moved = false; // update when move occurs, consider having a has moved map
    
    Pawn(piece_color c, position p) : Piece(PAWN, c, p) {}
    
    // NEED LEGAILTY CHECKS
    uint64_t get_moves(uint64_t white_pieces, uint64_t black_pieces) const override {
        uint64_t moves = 0ULL;
        int direction = (color == WHITE) ? 1 : -1;
        uint64_t occupied = white_pieces | black_pieces;
        
        // one space
        uint8_t pos_i = 8 * pos.row + pos.col + 1;
        if (!((occupied >> (pos_i + direction * 8)) & 1ULL)) {
            moves |= (1ULL << (pos_i + direction * 8));
            // two spaces
            if (!((occupied >> (pos_i + direction * 16)) & 1ULL) && !has_moved) { //update has_moved
                moves |= (1ULL << (pos_i + direction * 16));
            }
        }
        
        // captures, add en passant later
        uint64_t& opposing_pieces = (color == WHITE) ? black_pieces : white_pieces;
        int target_space1 = 8 * pos.row + pos.col + 1 + direction * 7;
        int target_space2 = 8 * pos.row + pos.col + 1 + direction * 9;
        if ((opposing_pieces >> target_space1) & 1ULL) {
            moves |= (1ULL << target_space1);
        }
        if ((opposing_pieces >> target_space2) & 1ULL) {
            moves |= (1ULL << target_space2);
        }
    }
};

class Chess_Game {
private:
    piece_color player_turn = WHITE;
    std::vector<std::vector<std::shared_ptr<Piece>>> board;
    
    // Sprites and textures stored separately
    std::unordered_map<piece_color, std::unordered_map<piece_type, sf::Texture>> piece_texture;
    std::unordered_map<piece_color, std::unordered_map<piece_type, sf::Sprite>> piece_sprite;
    //std::unordered_map<piece_type, sf::Sprite> white_sprite;
    //std::unordered_map<piece_type, sf::Sprite> black_sprite;

public:
    
    Chess_Game () : board(8, std::vector<std::shared_ptr<Piece>>(8, nullptr)) { // maybe pointers instead
        initialize_textures();
        initialize_game();
    }
    
    void initialize_game () {
        
        for (unsigned int col = 0; col < 8; col++) {
            board[1][col] = std::make_shared<Pawn>(WHITE, position{1, col});
        }
        
        for (unsigned int col = 0; col < 8; col++) {
            board[6][col] = std::make_shared<Pawn>(BLACK, position{6, col});
        }
        
    }
    
    void initialize_textures () {
        
        if (!piece_texture[WHITE][PAWN].loadFromFile("Pieces/wpawn.png")) {
            std::cout << "FAILED TO LOAD!" << std::endl;
        }
        piece_sprite[WHITE].emplace(PAWN, piece_texture[WHITE][PAWN]);
        
        if (!piece_texture[BLACK][PAWN].loadFromFile("Pieces/bpawn.png")) {
            std::cout << "FAILED TO LOAD!" << std::endl;
        }
        piece_sprite[BLACK].emplace(PAWN, piece_texture[BLACK][PAWN]);
        
        /*
        sf::Texture wpawn_texture;
        wpawn_texture.loadFromFile("chess_challenge/Pieces/wpawn.png");
        sf::Sprite wpawn(wpawn_texture);
        piece_sprite[WHITE][PAWN] = wpawn;
        
        sf::Texture bpawn_texture;
        bpawn_texture.loadFromFile("chess_challenge/Pieces/bpawn.png");
        sf::Sprite bpawn(bpawn_texture);
        piece_sprite[BLACK][PAWN] = bpawn;
         */
        
        // intiialize everything else
        
        for (auto& [color, sprites] : piece_sprite) {
            for (auto& [type, sprite] : sprites) {
                sprite.setScale(sf::Vector2f(80.f / sprite.getTexture().getSize().x,
                                             80.f / sprite.getTexture().getSize().y));
            }
        }
        
    }
    
    void handle_click (int row, int col) {
        // first check if promotion is pending/any other funky event
    }
    
    void draw (sf::RenderWindow& window) {
        // draw board
        for (int r = 0; r < 8; r++) {
            for(int c = 0; c < 8; c++) {
                
                sf::RectangleShape square(sf::Vector2f(80, 80));
                square.setPosition(sf::Vector2f(80 * c, 80 * r));
                square.setFillColor((r + c) % 2 == 0 ? sf::Color(94, 117, 135) : sf::Color(130, 161, 183));
                window.draw(square);
            
            }
        }
        
        // draw selected square + legal moves
        
        
        // draw pieces
        for (int c = 0; c < 8; c++) {
            for(int r = 0; r < 8; r++) {
                if(board[r][c] != nullptr) {
                    piece_color color = board[r][c]->color;
                    piece_type type = board[r][c]->type;
                    
                    sf::Sprite piece = piece_sprite.at(color).at(type); //throws if doesn't exist
                    piece.setPosition(sf::Vector2f(80 * c, 80 * r));
                    window.draw(piece);
                }
            }
        }
        
    }
    
};


// add has_moved to pawn, rook, king


// each piece stores all piece positions, a "has_moved" map, and allows moves to be generated piece by piece
// is there actually an advantage to having the bitmap stored together (better for summing evaluations?)
// maybe should be individual?
// don't even do classes?
/*
class Pawn : public Piece {
public:
    Pawn(piece_color c, uint64_t b) : Piece(PAWN, c, b) {}
    bool has_moved = false; // needs to be piece by piece
    
    uint64_t get_moves(uint64_t pos_b, uint64_t white_occupied, uint64_t black_occupied) {
        uint64_t moves = 0ULL;
        uint64_t occupied = white_occupied | black_occupied;
        
        int direction = (color == WHITE) ? 1 : -1;
        // need function that checks if you're under check after the move
        int pos_i = get_LSB(pos_b);
        // single move
        if (!((occupied >> (pos_i + direction * 8)) & 1ULL)) { // check legality
            moves |= (1ULL << (pos_i + direction * 8));
            // double move
            if (!((occupied >> (pos_i + direction * 16)) & 1ULL) && !has_moved) { //update has_moved
                moves |= (1ULL << (pos_i + direction * 16));
            }
        }
        //captures
        
    }
};
*/


int main() {
    Chess_Game game;
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(640, 640)), "Chess");
    
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (const auto* mouse_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                int col = mouse_pressed->position.x / 80;
                int row = mouse_pressed->position.y / 80;
                
                game.handle_click(row, col);
            }
            
            /*
            if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                int col = mouseButtonPressed->position.x / 80;
                int row = mouseButtonPressed->position.y / 80;
                
                if (game.isPromotionPending()) {
                    if (mouseButtonPressed->position.x >= 220 && mouseButtonPressed->position.x <= 420) {
                        int choice = (mouseButtonPressed->position.y - 200) / 80;
                        game.handlePromotion(choice);
                    }
                } else {
                    game.handleClick(row, col);
                }
            }
             */
        }

        window.clear();
        game.draw(window);
        window.display();
    }
    
    return 0;
}

